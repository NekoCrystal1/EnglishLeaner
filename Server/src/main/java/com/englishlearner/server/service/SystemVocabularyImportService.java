package com.englishlearner.server.service;

import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.book.SystemBookImportResponse;
import com.englishlearner.server.dto.book.SystemBookImportResult;
import com.englishlearner.server.model.Book;
import com.englishlearner.server.model.BookWord;
import com.englishlearner.server.model.VocabularyWord;
import com.englishlearner.server.repository.BookRepository;
import com.englishlearner.server.repository.BookWordRepository;
import com.englishlearner.server.repository.VocabularyWordRepository;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.core.io.ClassPathResource;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

@Service
public class SystemVocabularyImportService {
    private static final String INDEX_PATH = "vocabulary/ecdict/index.json";
    private static final int LOOKUP_CHUNK_SIZE = 500;

    private final BookRepository bookRepository;
    private final BookWordRepository bookWordRepository;
    private final VocabularyWordRepository vocabularyWordRepository;
    private final ObjectMapper objectMapper;

    public SystemVocabularyImportService(BookRepository bookRepository,
                                         BookWordRepository bookWordRepository,
                                         VocabularyWordRepository vocabularyWordRepository,
                                         ObjectMapper objectMapper) {
        this.bookRepository = bookRepository;
        this.bookWordRepository = bookWordRepository;
        this.vocabularyWordRepository = vocabularyWordRepository;
        this.objectMapper = objectMapper;
    }

    @Transactional
    public SystemBookImportResponse importSystemBooks(String bookKey) {
        JsonNode index = readClasspathJson(INDEX_PATH);
        List<SystemBookImportResult> results = new ArrayList<>();
        for (JsonNode bookRef : index.path("books")) {
            String key = text(bookRef, "key");
            if (bookKey != null && !bookKey.isBlank() && !bookKey.trim().equalsIgnoreCase(key)) {
                continue;
            }
            results.add(importBook(key, text(bookRef, "path")));
        }
        if (results.isEmpty()) {
            throw BusinessException.notFound("system vocabulary book not found");
        }

        int wordCount = results.stream().mapToInt(SystemBookImportResult::wordCount).sum();
        int relationCount = results.stream().mapToInt(SystemBookImportResult::relationCount).sum();
        return new SystemBookImportResponse(results.size(), wordCount, relationCount, results);
    }

    private SystemBookImportResult importBook(String key, String path) {
        if (!path.startsWith("vocabulary/ecdict/") || !path.endsWith(".json")) {
            throw BusinessException.badRequest("invalid system vocabulary resource path");
        }

        JsonNode payload = readClasspathJson(path);
        JsonNode wordNodes = payload.path("words");
        if (!wordNodes.isArray() || wordNodes.isEmpty()) {
            throw BusinessException.badRequest("system vocabulary book is empty");
        }

        Book book = upsertBook(payload, path, wordNodes.size());
        Map<String, VocabularyWord> existingWords = findExistingWords(wordNodes);
        List<VocabularyWord> savedWords = saveWords(wordNodes, existingWords);
        replaceBookWords(book.getId(), savedWords, payload.path("unitSize").asInt(20));

        book.setItemCount(savedWords.size());
        bookRepository.save(book);

        return new SystemBookImportResult(
                key,
                book.getId(),
                book.getTitle(),
                book.getLevel(),
                savedWords.size(),
                savedWords.size(),
                text(payload, "contentHash")
        );
    }

    private Book upsertBook(JsonNode payload, String path, int itemCount) {
        String title = text(payload, "title");
        Book book = bookRepository
                .findFirstByTitleAndSourceTypeAndOwnerUserIdIsNullAndDeletedFalse(title, "SYSTEM")
                .orElseGet(Book::new);
        book.setTitle(title);
        book.setBookType(defaultText(payload, "bookType", "WORD"));
        book.setSourceType("SYSTEM");
        book.setOwnerUserId(null);
        book.setLanguage(defaultText(payload, "language", "en"));
        book.setLevel(textOrNull(payload, "level"));
        book.setItemCount(itemCount);
        book.setContentStorageMode("SERVER");
        book.setClientContentRef("classpath:" + path);
        book.setContentHash(textOrNull(payload, "contentHash"));
        book.setStatus("ACTIVE");
        book.setRemark("Imported from ECDICT (MIT)");
        return bookRepository.save(book);
    }

    private Map<String, VocabularyWord> findExistingWords(JsonNode wordNodes) {
        List<String> normalizedWords = new ArrayList<>();
        for (JsonNode wordNode : wordNodes) {
            String normalized = defaultText(wordNode, "normalizedWord", text(wordNode, "word").toLowerCase());
            normalizedWords.add(normalized);
        }

        Map<String, VocabularyWord> existing = new LinkedHashMap<>();
        for (List<String> chunk : chunks(normalizedWords, LOOKUP_CHUNK_SIZE)) {
            for (VocabularyWord word : vocabularyWordRepository.findByNormalizedWordInAndDeletedFalse(chunk)) {
                existing.putIfAbsent(word.getNormalizedWord(), word);
            }
        }
        return existing;
    }

    private List<VocabularyWord> saveWords(JsonNode wordNodes, Map<String, VocabularyWord> existingWords) {
        List<VocabularyWord> words = new ArrayList<>();
        for (JsonNode wordNode : wordNodes) {
            String wordText = text(wordNode, "word");
            String normalized = defaultText(wordNode, "normalizedWord", wordText.toLowerCase());
            VocabularyWord word = existingWords.getOrDefault(normalized, new VocabularyWord());
            word.setWord(wordText);
            word.setNormalizedWord(normalized);
            word.setPhoneticUs(textOrNull(wordNode, "phonetic"));
            word.setTranslation(text(wordNode, "translation"));
            word.setPartOfSpeech(textOrNull(wordNode, "partOfSpeech"));
            word.setExampleSentence(textOrNull(wordNode, "exampleSentence"));
            word.setDifficulty(wordNode.path("difficulty").asInt(1));
            word.setExtJson(toExtJson(wordNode));
            word.setRemark("Imported from ECDICT (MIT)");
            words.add(word);
        }
        return vocabularyWordRepository.saveAll(words);
    }

    private void replaceBookWords(Long bookId, List<VocabularyWord> words, int unitSize) {
        bookWordRepository.deleteAllByBookId(bookId);
        List<BookWord> relations = new ArrayList<>();
        int safeUnitSize = Math.max(1, unitSize);
        for (int index = 0; index < words.size(); index++) {
            BookWord relation = new BookWord();
            relation.setBookId(bookId);
            relation.setWordId(words.get(index).getId());
            relation.setUnitNo((index / safeUnitSize) + 1);
            relation.setSortOrder(index + 1);
            relation.setRequired(true);
            relations.add(relation);
        }
        bookWordRepository.saveAll(relations);
    }

    private String toExtJson(JsonNode wordNode) {
        Map<String, Object> ext = new LinkedHashMap<>();
        ext.put("source", "ECDICT");
        copyIfPresent(wordNode, ext, "tags");
        copyIfPresent(wordNode, ext, "definition");
        copyIfPresent(wordNode, ext, "exchange");
        copyIfPresent(wordNode, ext, "bnc");
        copyIfPresent(wordNode, ext, "frq");
        try {
            return objectMapper.writeValueAsString(ext);
        } catch (IOException ex) {
            throw BusinessException.badRequest("failed to serialize vocabulary metadata");
        }
    }

    private void copyIfPresent(JsonNode source, Map<String, Object> target, String fieldName) {
        JsonNode node = source.get(fieldName);
        if (node == null || node.isNull()) {
            return;
        }
        if (node.isTextual() && node.asText().isBlank()) {
            return;
        }
        target.put(fieldName, objectMapper.convertValue(node, Object.class));
    }

    private JsonNode readClasspathJson(String path) {
        ClassPathResource resource = new ClassPathResource(path);
        try (InputStream inputStream = resource.getInputStream()) {
            return objectMapper.readTree(inputStream);
        } catch (IOException ex) {
            throw BusinessException.notFound("system vocabulary resource not found: " + path);
        }
    }

    private List<List<String>> chunks(List<String> values, int size) {
        List<List<String>> chunks = new ArrayList<>();
        for (int start = 0; start < values.size(); start += size) {
            chunks.add(values.subList(start, Math.min(start + size, values.size())));
        }
        return chunks;
    }

    private String text(JsonNode node, String fieldName) {
        String value = textOrNull(node, fieldName);
        if (value == null) {
            throw BusinessException.badRequest("missing required field: " + fieldName);
        }
        return value;
    }

    private String defaultText(JsonNode node, String fieldName, String fallback) {
        String value = textOrNull(node, fieldName);
        return value == null ? fallback : value;
    }

    private String textOrNull(JsonNode node, String fieldName) {
        JsonNode value = node.get(fieldName);
        if (value == null || value.isNull()) {
            return null;
        }
        String text = value.asText().trim();
        return text.isEmpty() ? null : text;
    }
}
