package com.englishlearner.server.service;

import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.book.BookResponse;
import com.englishlearner.server.dto.book.BookWordResponse;
import com.englishlearner.server.dto.book.CreateBookRequest;
import com.englishlearner.server.model.Book;
import com.englishlearner.server.model.BookWord;
import com.englishlearner.server.model.VocabularyWord;
import com.englishlearner.server.repository.BookRepository;
import com.englishlearner.server.repository.BookWordRepository;
import com.englishlearner.server.repository.VocabularyWordRepository;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;

@Service
public class BookService {
    private final BookRepository bookRepository;
    private final BookWordRepository bookWordRepository;
    private final VocabularyWordRepository vocabularyWordRepository;

    public BookService(BookRepository bookRepository,
                       BookWordRepository bookWordRepository,
                       VocabularyWordRepository vocabularyWordRepository) {
        this.bookRepository = bookRepository;
        this.bookWordRepository = bookWordRepository;
        this.vocabularyWordRepository = vocabularyWordRepository;
    }

    public List<BookResponse> listVisibleBooks(Long userId, String bookType) {
        return bookRepository.findVisibleBooks(userId, trimToNull(bookType)).stream()
                .map(this::toBookResponse)
                .toList();
    }

    public BookResponse getVisibleBook(Long userId, Long bookId) {
        Book book = findVisibleBook(userId, bookId);
        return toBookResponse(book);
    }

    @Transactional
    public BookResponse createUserBook(Long userId, CreateBookRequest request) {
        Book book = new Book();
        book.setTitle(request.title().trim());
        book.setBookType(request.bookType().trim().toUpperCase());
        book.setSourceType("USER_UPLOAD");
        book.setOwnerUserId(userId);
        book.setLanguage(defaultValue(request.language(), "en"));
        book.setLevel(trimToNull(request.level()));
        book.setCoverUrl(trimToNull(request.coverUrl()));
        book.setItemCount(request.itemCount() == null ? 0 : Math.max(0, request.itemCount()));
        book.setContentStorageMode("CLIENT_ONLY");
        book.setClientContentRef(trimToNull(request.clientContentRef()));
        book.setContentHash(trimToNull(request.contentHash()));
        book.setStatus("ACTIVE");
        return toBookResponse(bookRepository.save(book));
    }

    public List<BookWordResponse> listBookWords(Long userId, Long bookId) {
        Book book = findVisibleBook(userId, bookId);
        List<BookWord> relations = bookWordRepository.findByBookIdAndDeletedFalseOrderBySortOrderAscIdAsc(book.getId());
        if (relations.isEmpty()) {
            return List.of();
        }

        List<Long> wordIds = relations.stream().map(BookWord::getWordId).toList();
        Map<Long, VocabularyWord> words = vocabularyWordRepository.findByIdInAndDeletedFalse(wordIds).stream()
                .collect(Collectors.toMap(VocabularyWord::getId, Function.identity(), (a, b) -> a, LinkedHashMap::new));

        List<BookWordResponse> responses = new ArrayList<>();
        for (BookWord relation : relations) {
            VocabularyWord word = words.get(relation.getWordId());
            if (word == null) {
                continue;
            }
            responses.add(toBookWordResponse(relation, word));
        }
        return responses;
    }

    public Book findVisibleBook(Long userId, Long bookId) {
        Book book = bookRepository.findByIdAndDeletedFalse(bookId)
                .orElseThrow(() -> BusinessException.notFound("book not found"));
        if (!"ACTIVE".equals(book.getStatus())) {
            throw BusinessException.notFound("book not found");
        }
        if (book.getOwnerUserId() != null && !book.getOwnerUserId().equals(userId)) {
            throw BusinessException.notFound("book not found");
        }
        return book;
    }

    public BookResponse toBookResponse(Book book) {
        return new BookResponse(
                book.getId(),
                book.getTitle(),
                book.getBookType(),
                book.getSourceType(),
                book.getOwnerUserId(),
                book.getLanguage(),
                book.getLevel(),
                book.getCoverUrl(),
                book.getItemCount(),
                book.getContentStorageMode(),
                book.getClientContentRef(),
                book.getContentHash(),
                book.getStatus()
        );
    }

    private BookWordResponse toBookWordResponse(BookWord relation, VocabularyWord word) {
        return new BookWordResponse(
                word.getId(),
                word.getWord(),
                word.getTranslation(),
                word.getPhoneticUs(),
                word.getPhoneticUk(),
                word.getPartOfSpeech(),
                word.getExampleSentence(),
                word.getDifficulty(),
                relation.getUnitNo(),
                relation.getSortOrder()
        );
    }

    private String defaultValue(String value, String fallback) {
        String trimmed = trimToNull(value);
        return trimmed == null ? fallback : trimmed;
    }

    private String trimToNull(String value) {
        if (value == null) {
            return null;
        }
        String trimmed = value.trim();
        return trimmed.isEmpty() ? null : trimmed;
    }
}
