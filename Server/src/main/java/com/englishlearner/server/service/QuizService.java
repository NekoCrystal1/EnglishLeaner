package com.englishlearner.server.service;

import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.quiz.QuizQuestionResponse;
import com.englishlearner.server.dto.quiz.SubmitAnswerRequest;
import com.englishlearner.server.dto.quiz.SubmitAnswerResponse;
import com.englishlearner.server.dto.ranking.RankingItemResponse;
import com.englishlearner.server.model.StudyLog;
import com.englishlearner.server.model.UserAccount;
import com.englishlearner.server.model.Vocabulary;
import com.englishlearner.server.repository.StudyLogRepository;
import com.englishlearner.server.repository.UserAccountRepository;
import com.englishlearner.server.repository.VocabularyRepository;
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
    private static final int CORRECT_SCORE = 10;

    private final VocabularyRepository vocabularyRepository;
    private final UserAccountRepository userAccountRepository;
    private final StudyLogRepository studyLogRepository;
    private final Random random = new SecureRandom();

    public QuizService(VocabularyRepository vocabularyRepository,
                       UserAccountRepository userAccountRepository,
                       StudyLogRepository studyLogRepository) {
        this.vocabularyRepository = vocabularyRepository;
        this.userAccountRepository = userAccountRepository;
        this.studyLogRepository = studyLogRepository;
    }

    public QuizQuestionResponse nextQuestion() {
        long total = vocabularyRepository.countByDeletedFalse();
        if (total == 0L) {
            throw BusinessException.notFound("no vocabulary available");
        }

        int offset = random.nextInt((int) Math.min(total, Integer.MAX_VALUE));
        Page<Vocabulary> page = vocabularyRepository.findByDeletedFalse(
                PageRequest.of(offset, 1, Sort.by(Sort.Direction.ASC, "id")));
        if (page.isEmpty()) {
            page = vocabularyRepository.findByDeletedFalse(
                    PageRequest.of(0, 1, Sort.by(Sort.Direction.ASC, "id")));
        }
        if (page.isEmpty()) {
            throw BusinessException.notFound("no vocabulary available");
        }

        Vocabulary vocabulary = page.getContent().get(0);
        List<String> options = buildOptions(vocabulary);
        return new QuizQuestionResponse(vocabulary.getId(), vocabulary.getWord(), options);
    }

    public SubmitAnswerResponse submitAnswer(Long userId, SubmitAnswerRequest request) {
        UserAccount user = userAccountRepository.findByIdAndDeletedFalse(userId)
                .orElseThrow(() -> BusinessException.unauthorized("user not found or session expired"));
        Vocabulary vocabulary = vocabularyRepository.findByIdAndDeletedFalse(request.wordId())
                .orElseThrow(() -> BusinessException.notFound("question not found"));

        String selectedAnswer = request.selectedAnswer().trim();
        boolean correct = vocabulary.getTranslation().equals(selectedAnswer);
        int scoreDelta = correct ? CORRECT_SCORE : 0;

        StudyLog log = new StudyLog();
        log.setUserId(user.getId());
        log.setWordId(vocabulary.getId());
        log.setSelectedAnswer(selectedAnswer);
        log.setCorrect(correct);
        log.setScoreDelta(scoreDelta);
        studyLogRepository.save(log);

        if (correct) {
            user.setTotalScore(user.getTotalScore() + scoreDelta);
            userAccountRepository.save(user);
        }

        return new SubmitAnswerResponse(
                correct,
                vocabulary.getTranslation(),
                scoreDelta,
                user.getTotalScore()
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

    private List<String> buildOptions(Vocabulary vocabulary) {
        Set<String> unique = new LinkedHashSet<>();
        addIfNotBlank(unique, vocabulary.getTranslation());
        addIfNotBlank(unique, vocabulary.getOption1());
        addIfNotBlank(unique, vocabulary.getOption2());
        addIfNotBlank(unique, vocabulary.getOption3());

        List<Vocabulary> others = vocabularyRepository.findByIdNotAndDeletedFalse(vocabulary.getId());
        Collections.shuffle(others, random);
        for (Vocabulary item : others) {
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
