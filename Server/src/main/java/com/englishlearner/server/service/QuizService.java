package com.englishlearner.server.service;

import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.quiz.QuizQuestionResponse;
import com.englishlearner.server.dto.quiz.SubmitAnswerRequest;
import com.englishlearner.server.dto.quiz.SubmitAnswerResponse;
import com.englishlearner.server.dto.ranking.RankingItemResponse;
import com.englishlearner.server.model.UserAccount;
import com.englishlearner.server.model.VocabularyWord;
import com.englishlearner.server.repository.UserAccountRepository;
import com.englishlearner.server.repository.VocabularyWordRepository;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Sort;
import org.springframework.stereotype.Service;

import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Random;
import java.util.Set;

@Service
public class QuizService {
    private static final int OPTION_SIZE = 4;

    private final VocabularyWordRepository vocabularyWordRepository;
    private final UserAccountRepository userAccountRepository;
    private final WordLearningService wordLearningService;
    private final Random random = new SecureRandom();

    public QuizService(VocabularyWordRepository vocabularyWordRepository,
                       UserAccountRepository userAccountRepository,
                       WordLearningService wordLearningService) {
        this.vocabularyWordRepository = vocabularyWordRepository;
        this.userAccountRepository = userAccountRepository;
        this.wordLearningService = wordLearningService;
    }

    public QuizQuestionResponse nextQuestion() {
        // 使用随机偏移保持接口简单，同时让题目分布到整个单词表。
        long total = vocabularyWordRepository.countByDeletedFalse();
        if (total == 0L) {
            throw BusinessException.notFound("no vocabulary available");
        }

        int offset = random.nextInt((int) Math.min(total, Integer.MAX_VALUE));
        Page<VocabularyWord> page = vocabularyWordRepository.findByDeletedFalse(
                PageRequest.of(offset, 1, Sort.by(Sort.Direction.ASC, "id")));
        if (page.isEmpty()) {
            page = vocabularyWordRepository.findByDeletedFalse(
                    PageRequest.of(0, 1, Sort.by(Sort.Direction.ASC, "id")));
        }
        if (page.isEmpty()) {
            throw BusinessException.notFound("no vocabulary available");
        }

        VocabularyWord vocabulary = page.getContent().get(0);
        List<String> options = buildOptions(vocabulary);
        return new QuizQuestionResponse(vocabulary.getId(), vocabulary.getWord(), options);
    }

    public SubmitAnswerResponse submitAnswer(Long userId, SubmitAnswerRequest request) {
        VocabularyWord vocabulary = vocabularyWordRepository.findByIdAndDeletedFalse(request.wordId())
                .orElseThrow(() -> BusinessException.notFound("question not found"));

        String selectedAnswer = request.selectedAnswer().trim();
        boolean correct = vocabulary.getTranslation().equals(selectedAnswer);
        var event = wordLearningService.recordQuizAnswer(userId, vocabulary, selectedAnswer, correct);

        return new SubmitAnswerResponse(
                correct,
                vocabulary.getTranslation(),
                event.scoreDelta(),
                event.totalScore()
        );
    }

    public List<RankingItemResponse> topRanking(int limit) {
        int safeLimit = Math.max(1, Math.min(limit, 100));
        List<UserAccount> users = userAccountRepository.findTopByScore(PageRequest.of(0, safeLimit));
        List<RankingItemResponse> rankings = new ArrayList<>(users.size());

        int rank = 1;
        for (UserAccount user : users) {
            rankings.add(new RankingItemResponse(
                    rank++,
                    user.getId(),
                    user.getUsername(),
                    user.getTotalScore()
            ));
        }
        return rankings;
    }

    private List<String> buildOptions(VocabularyWord vocabulary) {
        // 选项始终包含正确答案，再用其他单词的唯一翻译补足。
        Set<String> unique = new LinkedHashSet<>();
        addIfNotBlank(unique, vocabulary.getTranslation());

        List<VocabularyWord> others = vocabularyWordRepository.findByIdNotAndDeletedFalse(vocabulary.getId());
        Collections.shuffle(others, random);
        for (VocabularyWord item : others) {
            addIfNotBlank(unique, item.getTranslation());
            if (unique.size() >= 12) {
                break;
            }
        }

        String correctAnswer = vocabulary.getTranslation();
        List<String> otherCandidates = new ArrayList<>(unique);
        otherCandidates.remove(correctAnswer);
        Collections.shuffle(otherCandidates, random);

        List<String> options = new ArrayList<>(OPTION_SIZE);
        options.add(correctAnswer);
        for (String candidate : otherCandidates) {
            if (options.size() >= OPTION_SIZE) {
                break;
            }
            options.add(candidate);
        }
        while (options.size() < OPTION_SIZE) {
            options.add("N/A");
        }
        Collections.shuffle(options, random);
        return options;
    }

    private void addIfNotBlank(Set<String> values, String value) {
        if (value == null) {
            return;
        }
        String trimmed = value.trim();
        if (!trimmed.isEmpty()) {
            values.add(trimmed);
        }
    }
}
