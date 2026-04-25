package com.englishlearner.server.service;

import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.word.CreateWordPlanRequest;
import com.englishlearner.server.dto.word.DailyStudySummaryResponse;
import com.englishlearner.server.dto.word.SubmitWordStudyEventRequest;
import com.englishlearner.server.dto.word.UpdateWordPlanRequest;
import com.englishlearner.server.dto.word.WordLearningItemResponse;
import com.englishlearner.server.dto.word.WordPlanResponse;
import com.englishlearner.server.dto.word.WordStudyEventResponse;
import com.englishlearner.server.model.Book;
import com.englishlearner.server.model.BookWord;
import com.englishlearner.server.model.DailyStudySummary;
import com.englishlearner.server.model.ScoreTransaction;
import com.englishlearner.server.model.UserAccount;
import com.englishlearner.server.model.UserWordPlan;
import com.englishlearner.server.model.UserWordState;
import com.englishlearner.server.model.VocabularyWord;
import com.englishlearner.server.model.WordStudyEvent;
import com.englishlearner.server.repository.BookRepository;
import com.englishlearner.server.repository.BookWordRepository;
import com.englishlearner.server.repository.DailyStudySummaryRepository;
import com.englishlearner.server.repository.ScoreTransactionRepository;
import com.englishlearner.server.repository.UserAccountRepository;
import com.englishlearner.server.repository.UserWordPlanRepository;
import com.englishlearner.server.repository.UserWordStateRepository;
import com.englishlearner.server.repository.VocabularyWordRepository;
import com.englishlearner.server.repository.WordStudyEventRepository;
import org.springframework.data.domain.PageRequest;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.function.Function;
import java.util.stream.Collectors;

@Service
public class WordLearningService {
    private static final int CORRECT_SCORE = 10;
    private static final double MIN_EASE_FACTOR = 1.3;
    private static final double MAX_EASE_FACTOR = 3.0;
    private static final int MAX_INTERVAL_DAYS = 365;

    private final UserAccountRepository userAccountRepository;
    private final BookRepository bookRepository;
    private final BookWordRepository bookWordRepository;
    private final VocabularyWordRepository vocabularyWordRepository;
    private final UserWordPlanRepository userWordPlanRepository;
    private final UserWordStateRepository userWordStateRepository;
    private final WordStudyEventRepository wordStudyEventRepository;
    private final ScoreTransactionRepository scoreTransactionRepository;
    private final DailyStudySummaryRepository dailyStudySummaryRepository;

    public WordLearningService(UserAccountRepository userAccountRepository,
                               BookRepository bookRepository,
                               BookWordRepository bookWordRepository,
                               VocabularyWordRepository vocabularyWordRepository,
                               UserWordPlanRepository userWordPlanRepository,
                               UserWordStateRepository userWordStateRepository,
                               WordStudyEventRepository wordStudyEventRepository,
                               ScoreTransactionRepository scoreTransactionRepository,
                               DailyStudySummaryRepository dailyStudySummaryRepository) {
        this.userAccountRepository = userAccountRepository;
        this.bookRepository = bookRepository;
        this.bookWordRepository = bookWordRepository;
        this.vocabularyWordRepository = vocabularyWordRepository;
        this.userWordPlanRepository = userWordPlanRepository;
        this.userWordStateRepository = userWordStateRepository;
        this.wordStudyEventRepository = wordStudyEventRepository;
        this.scoreTransactionRepository = scoreTransactionRepository;
        this.dailyStudySummaryRepository = dailyStudySummaryRepository;
    }

    @Transactional
    public WordPlanResponse createPlan(Long userId, CreateWordPlanRequest request) {
        Book book = findVisibleWordBook(userId, request.bookId());

        // 同一用户只保留一个激活中的单词计划；创建新计划前先暂停旧计划。
        userWordPlanRepository.findFirstByUserIdAndStatusAndDeletedFalseOrderByIdDesc(userId, "ACTIVE")
                .ifPresent(active -> {
                    active.setStatus("PAUSED");
                    userWordPlanRepository.save(active);
                });

        UserWordPlan plan = new UserWordPlan();
        plan.setUserId(userId);
        plan.setBookId(book.getId());
        plan.setDailyNewCount(request.dailyNewCount() == null ? 10 : request.dailyNewCount());
        plan.setDailyReviewCount(request.dailyReviewCount() == null ? 20 : request.dailyReviewCount());
        plan.setCurrentPosition(0);
        plan.setReviewAlgorithm("SM2_ADAPTIVE");
        plan.setStatus("ACTIVE");
        UserWordPlan saved = userWordPlanRepository.save(plan);
        return toPlanResponse(saved, book);
    }

    @Transactional
    public WordPlanResponse getOrCreateCurrentPlan(Long userId) {
        Optional<UserWordPlan> activePlan = userWordPlanRepository
                .findFirstByUserIdAndStatusAndDeletedFalseOrderByIdDesc(userId, "ACTIVE");
        if (activePlan.isPresent()) {
            UserWordPlan plan = activePlan.get();
            Book book = bookRepository.findByIdAndDeletedFalse(plan.getBookId())
                    .orElseThrow(() -> BusinessException.notFound("book not found"));
            return toPlanResponse(plan, book);
        }

        Book defaultBook = bookRepository
                .findFirstByBookTypeAndStatusAndOwnerUserIdIsNullAndDeletedFalseOrderByIdAsc("WORD", "ACTIVE")
                .orElseThrow(() -> BusinessException.notFound("no word book available"));
        return createPlan(userId, new CreateWordPlanRequest(defaultBook.getId(), 10, 20));
    }

    public List<WordPlanResponse> listPlans(Long userId) {
        return userWordPlanRepository.findByUserIdAndDeletedFalseOrderByCreatedAtDesc(userId).stream()
                .map(plan -> {
                    Book book = bookRepository.findByIdAndDeletedFalse(plan.getBookId()).orElse(null);
                    return toPlanResponse(plan, book);
                })
                .toList();
    }

    @Transactional
    public WordPlanResponse updatePlan(Long userId, Long planId, UpdateWordPlanRequest request) {
        UserWordPlan plan = userWordPlanRepository.findByIdAndUserIdAndDeletedFalse(planId, userId)
                .orElseThrow(() -> BusinessException.notFound("word plan not found"));
        if (request.dailyNewCount() != null) {
            plan.setDailyNewCount(request.dailyNewCount());
        }
        if (request.dailyReviewCount() != null) {
            plan.setDailyReviewCount(request.dailyReviewCount());
        }
        String status = trimToNull(request.status());
        if (status != null) {
            plan.setStatus(status.toUpperCase());
            if ("FINISHED".equals(plan.getStatus()) && plan.getFinishedOn() == null) {
                plan.setFinishedOn(LocalDate.now());
            }
        }
        UserWordPlan saved = userWordPlanRepository.save(plan);
        Book book = bookRepository.findByIdAndDeletedFalse(saved.getBookId()).orElse(null);
        return toPlanResponse(saved, book);
    }

    public List<WordLearningItemResponse> getDueWords(Long userId, Long planId) {
        UserWordPlan plan = resolvePlan(userId, planId);
        LocalDateTime now = LocalDateTime.now();
        List<WordLearningItemResponse> items = new ArrayList<>();

        // 复习额度优先，避免过期复习词被当天新词目标挤掉。
        List<UserWordState> reviewStates = userWordStateRepository.findDueReviewStates(
                userId,
                plan.getId(),
                now,
                PageRequest.of(0, Math.max(1, plan.getDailyReviewCount()))
        );
        items.addAll(toReviewItems(reviewStates));

        int newCount = newWordsRemainingToday(userId, plan);
        if (newCount > 0) {
            List<BookWord> newRelations = bookWordRepository.findUnseenWords(
                    userId,
                    plan.getId(),
                    plan.getBookId(),
                    PageRequest.of(0, newCount));
            items.addAll(toNewItems(newRelations));
        }
        return items;
    }

    private int newWordsRemainingToday(Long userId, UserWordPlan plan) {
        int dailyNewCount = Math.max(0, plan.getDailyNewCount());
        int learnedToday = dailyStudySummaryRepository
                .findByUserIdAndStudyDateAndDeletedFalse(userId, LocalDate.now())
                .map(DailyStudySummary::getNewWords)
                .orElse(0);
        return Math.max(0, dailyNewCount - learnedToday);
    }

    @Transactional
    public WordStudyEventResponse submitWordEvent(Long userId, SubmitWordStudyEventRequest request) {
        UserWordPlan plan = resolvePlan(userId, request.planId());
        VocabularyWord word = vocabularyWordRepository.findByIdAndDeletedFalse(request.wordId())
                .orElseThrow(() -> BusinessException.notFound("word not found"));
        String activityType = normalizeActivityType(defaultValue(request.activityType(), "REVIEW"));
        int quality = normalizeQuality(request.quality(), request.correct());
        return recordWordAnswer(
                userId,
                plan,
                word,
                activityType,
                request.selectedAnswer(),
                quality,
                request.durationMs(),
                request.clientEventId());
    }

    @Transactional
    public WordStudyEventResponse recordQuizAnswer(Long userId,
                                                   VocabularyWord word,
                                                   String selectedAnswer,
                                                   boolean correct) {
        return recordWordAnswer(userId, null, word, "QUIZ", selectedAnswer, normalizeQuality(null, correct), null, null);
    }

    public DailyStudySummaryResponse getTodaySummary(Long userId) {
        LocalDate today = LocalDate.now();
        DailyStudySummary summary = dailyStudySummaryRepository.findByUserIdAndStudyDateAndDeletedFalse(userId, today)
                .orElseGet(() -> emptySummary(userId, today));
        return toDailySummaryResponse(summary);
    }

    private WordStudyEventResponse recordWordAnswer(Long userId,
                                                    UserWordPlan plan,
                                                    VocabularyWord word,
                                                    String activityType,
                                                    String selectedAnswer,
                                                    int quality,
                                                    Integer durationMs,
                                                    String clientEventId) {
        // clientEventId 用于保证重试幂等，适配客户端超时后的重复提交。
        if (clientEventId != null && !clientEventId.isBlank()) {
            Optional<WordStudyEvent> existing = wordStudyEventRepository
                    .findByUserIdAndClientEventIdAndDeletedFalse(userId, clientEventId.trim());
            if (existing.isPresent()) {
                WordStudyEvent event = existing.get();
                UserWordState state = event.getPlanId() == null ? null : userWordStateRepository
                        .findByUserIdAndPlanIdAndWordIdAndDeletedFalse(userId, event.getPlanId(), event.getWordId())
                        .orElse(null);
                UserAccount user = findUser(userId);
                return new WordStudyEventResponse(
                        event.getId(),
                        event.getPlanId(),
                        event.getWordId(),
                        state == null ? null : state.getState(),
                        state == null ? null : state.getMasteryLevel(),
                        event.getScoreDelta(),
                        user.getTotalScore(),
                        state == null ? null : state.getNextReviewAt());
            }
        }

        UserAccount user = findUser(userId);
        boolean correct = quality >= 3;
        int scoreDelta = correct ? CORRECT_SCORE : 0;

        WordStudyEvent event = new WordStudyEvent();
        event.setUserId(userId);
        event.setPlanId(plan == null ? null : plan.getId());
        event.setBookId(plan == null ? null : plan.getBookId());
        event.setWordId(word.getId());
        event.setActivityType(activityType);
        event.setSelectedAnswer(trimToNull(selectedAnswer));
        event.setCorrect(correct);
        event.setScoreDelta(scoreDelta);
        event.setDurationMs(durationMs);
        event.setClientEventId(trimToNull(clientEventId));
        event.setExtJson("{\"quality\":" + quality + "}");
        WordStudyEvent savedEvent = wordStudyEventRepository.save(event);

        UserWordState state = null;
        if (plan != null) {
            state = updateWordState(userId, plan, word.getId(), quality);
            updatePlanPosition(plan, word.getId());
        }

        if (scoreDelta != 0) {
            user.setTotalScore(user.getTotalScore() + scoreDelta);
            ScoreTransaction transaction = new ScoreTransaction();
            transaction.setUserId(userId);
            transaction.setSourceType("WORD_EVENT");
            transaction.setSourceId(savedEvent.getId());
            transaction.setScoreDelta(scoreDelta);
            transaction.setBalanceAfter(user.getTotalScore());
            transaction.setReason(activityType);
            scoreTransactionRepository.save(transaction);
        }
        touchStudyDay(user);
        userAccountRepository.save(user);
        updateDailySummary(userId, activityType, correct, scoreDelta, durationMs);

        return new WordStudyEventResponse(
                savedEvent.getId(),
                savedEvent.getPlanId(),
                savedEvent.getWordId(),
                state == null ? null : state.getState(),
                state == null ? null : state.getMasteryLevel(),
                scoreDelta,
                user.getTotalScore(),
                state == null ? null : state.getNextReviewAt());
    }

    private UserWordState updateWordState(Long userId, UserWordPlan plan, Long wordId, int quality) {
        LocalDateTime now = LocalDateTime.now();
        UserWordState state = userWordStateRepository
                .findByUserIdAndPlanIdAndWordIdAndDeletedFalse(userId, plan.getId(), wordId)
                .orElseGet(() -> {
                    UserWordState created = new UserWordState();
                    created.setUserId(userId);
                    created.setPlanId(plan.getId());
                    created.setBookId(plan.getBookId());
                    created.setWordId(wordId);
                    created.setState("NEW");
                    created.setFirstSeenAt(now);
                    return created;
                });

        boolean correct = quality >= 3;
        double nextEaseFactor = nextEaseFactor(state.getEaseFactor(), quality);
        int previousIntervalDays = state.getIntervalDays() == null ? 0 : state.getIntervalDays();

        state.setExposureCount(state.getExposureCount() + 1);
        state.setLastAnsweredAt(now);
        state.setEaseFactor(nextEaseFactor);
        if (correct) {
            state.setCorrectCount(state.getCorrectCount() + 1);
            state.setMasteryLevel(nextMasteryLevel(state.getMasteryLevel(), quality, previousIntervalDays));
            state.setIntervalDays(nextIntervalDays(previousIntervalDays, nextEaseFactor, quality));
            state.setState(state.getMasteryLevel() >= 5 ? "MASTERED" : "REVIEWING");
            state.setNextReviewAt(now.plusDays(state.getIntervalDays()));
        } else {
            state.setWrongCount(state.getWrongCount() + 1);
            state.setMasteryLevel(Math.max(0, state.getMasteryLevel() - (quality <= 1 ? 2 : 1)));
            state.setIntervalDays(0);
            state.setState("LEARNING");
            state.setNextReviewAt(now.plusMinutes(learningDelayMinutes(quality)));
        }
        state.setSyncVersion(state.getSyncVersion() + 1);
        return userWordStateRepository.save(state);
    }

    private int nextIntervalDays(int previousIntervalDays, double easeFactor, int quality) {
        if (previousIntervalDays <= 0) {
            return quality >= 5 ? 2 : 1;
        }
        if (previousIntervalDays == 1) {
            if (quality >= 5) {
                return 6;
            }
            return quality == 4 ? 4 : 2;
        }

        double qualityMultiplier = switch (quality) {
            case 3 -> 0.75;
            case 5 -> 1.25;
            default -> 1.0;
        };
        int interval = (int) Math.round(previousIntervalDays * easeFactor * qualityMultiplier);
        return Math.max(previousIntervalDays + 1, Math.min(MAX_INTERVAL_DAYS, interval));
    }

    private double nextEaseFactor(Double currentEaseFactor, int quality) {
        double current = currentEaseFactor == null ? 2.5 : currentEaseFactor;
        double adjusted = current + (0.1 - (5 - quality) * (0.08 + (5 - quality) * 0.02));
        return Math.max(MIN_EASE_FACTOR, Math.min(MAX_EASE_FACTOR, adjusted));
    }

    private int nextMasteryLevel(Integer currentMasteryLevel, int quality, int previousIntervalDays) {
        int current = currentMasteryLevel == null ? 0 : currentMasteryLevel;
        int intervalBonus = previousIntervalDays >= 14 ? 1 : 0;
        int qualityBonus = quality >= 5 ? 2 : 1;
        return Math.min(5, current + qualityBonus + intervalBonus);
    }

    private int learningDelayMinutes(int quality) {
        if (quality <= 1) {
            return 10;
        }
        return 30;
    }

    private void updatePlanPosition(UserWordPlan plan, Long wordId) {
        bookWordRepository.findByBookIdAndWordIdAndDeletedFalse(plan.getBookId(), wordId)
                .ifPresent(relation -> {
                    if (relation.getSortOrder() > plan.getCurrentPosition()) {
                        plan.setCurrentPosition(relation.getSortOrder());
                        userWordPlanRepository.save(plan);
                    }
                });
    }

    private void updateDailySummary(Long userId,
                                    String activityType,
                                    boolean correct,
                                    int scoreDelta,
                                    Integer durationMs) {
        LocalDate today = LocalDate.now();
        DailyStudySummary summary = dailyStudySummaryRepository.findByUserIdAndStudyDateAndDeletedFalse(userId, today)
                .orElseGet(() -> emptySummary(userId, today));
        if ("NEW".equals(activityType)) {
            summary.setNewWords(summary.getNewWords() + 1);
        } else if ("REVIEW".equals(activityType)) {
            summary.setReviewWords(summary.getReviewWords() + 1);
        }
        if (correct) {
            summary.setCorrectCount(summary.getCorrectCount() + 1);
        } else {
            summary.setWrongCount(summary.getWrongCount() + 1);
        }
        if (durationMs != null && durationMs > 0) {
            summary.setStudySeconds(summary.getStudySeconds() + (durationMs / 1000));
        }
        summary.setScoreDelta(summary.getScoreDelta() + scoreDelta);
        summary.setCompletedTasks(summary.getCompletedTasks() + 1);
        dailyStudySummaryRepository.save(summary);
    }

    private DailyStudySummary emptySummary(Long userId, LocalDate studyDate) {
        DailyStudySummary summary = new DailyStudySummary();
        summary.setUserId(userId);
        summary.setStudyDate(studyDate);
        return summary;
    }

    private void touchStudyDay(UserAccount user) {
        LocalDate today = LocalDate.now();
        if (user.getLastStudyDate() == null || user.getLastStudyDate().isBefore(today)) {
            user.setStudyDays(user.getStudyDays() + 1);
            user.setLastStudyDate(today);
        }
    }

    private UserWordPlan resolvePlan(Long userId, Long planId) {
        if (planId != null) {
            return userWordPlanRepository.findByIdAndUserIdAndDeletedFalse(planId, userId)
                    .orElseThrow(() -> BusinessException.notFound("word plan not found"));
        }
        return userWordPlanRepository.findFirstByUserIdAndStatusAndDeletedFalseOrderByIdDesc(userId, "ACTIVE")
                .orElseGet(() -> {
                    WordPlanResponse response = getOrCreateCurrentPlan(userId);
                    return userWordPlanRepository.findByIdAndUserIdAndDeletedFalse(response.planId(), userId)
                            .orElseThrow(() -> BusinessException.notFound("word plan not found"));
                });
    }

    private Book findVisibleWordBook(Long userId, Long bookId) {
        Book book = bookRepository.findByIdAndDeletedFalse(bookId)
                .orElseThrow(() -> BusinessException.notFound("book not found"));
        if (!"WORD".equals(book.getBookType()) || !"ACTIVE".equals(book.getStatus())) {
            throw BusinessException.badRequest("book is not an active word book");
        }
        if (book.getOwnerUserId() != null && !book.getOwnerUserId().equals(userId)) {
            throw BusinessException.notFound("book not found");
        }
        return book;
    }

    private UserAccount findUser(Long userId) {
        return userAccountRepository.findByIdAndDeletedFalse(userId)
                .orElseThrow(() -> BusinessException.unauthorized("user not found or session expired"));
    }

    private WordPlanResponse toPlanResponse(UserWordPlan plan, Book book) {
        LocalDateTime now = LocalDateTime.now();
        long learnedWords = userWordStateRepository.countByUserIdAndPlanIdAndDeletedFalse(plan.getUserId(), plan.getId());
        long masteredWords = userWordStateRepository.countByUserIdAndPlanIdAndStateAndDeletedFalse(
                plan.getUserId(),
                plan.getId(),
                "MASTERED");
        long dueWords = userWordStateRepository.countDueReviewStates(plan.getUserId(), plan.getId(), now);
        long totalWords = bookWordRepository.countByBookIdAndDeletedFalse(plan.getBookId());
        return new WordPlanResponse(
                plan.getId(),
                plan.getBookId(),
                book == null ? null : book.getTitle(),
                plan.getDailyNewCount(),
                plan.getDailyReviewCount(),
                plan.getCurrentPosition(),
                plan.getReviewAlgorithm(),
                plan.getStartedOn(),
                plan.getFinishedOn(),
                plan.getStatus(),
                learnedWords,
                masteredWords,
                dueWords,
                totalWords);
    }

    private List<WordLearningItemResponse> toReviewItems(List<UserWordState> states) {
        if (states.isEmpty()) {
            return List.of();
        }
        List<Long> wordIds = states.stream().map(UserWordState::getWordId).toList();
        Map<Long, VocabularyWord> words = vocabularyWordRepository.findByIdInAndDeletedFalse(wordIds).stream()
                .collect(Collectors.toMap(VocabularyWord::getId, Function.identity(), (a, b) -> a, LinkedHashMap::new));
        List<WordLearningItemResponse> responses = new ArrayList<>();
        for (UserWordState state : states) {
            VocabularyWord word = words.get(state.getWordId());
            if (word != null) {
                responses.add(toLearningItem(word, "REVIEW", state));
            }
        }
        return responses;
    }

    private List<WordLearningItemResponse> toNewItems(List<BookWord> relations) {
        if (relations.isEmpty()) {
            return List.of();
        }
        List<Long> wordIds = relations.stream().map(BookWord::getWordId).toList();
        Map<Long, VocabularyWord> words = vocabularyWordRepository.findByIdInAndDeletedFalse(wordIds).stream()
                .collect(Collectors.toMap(VocabularyWord::getId, Function.identity(), (a, b) -> a, LinkedHashMap::new));
        List<WordLearningItemResponse> responses = new ArrayList<>();
        for (BookWord relation : relations) {
            VocabularyWord word = words.get(relation.getWordId());
            if (word != null) {
                responses.add(toLearningItem(word, "NEW", null));
            }
        }
        return responses;
    }

    private WordLearningItemResponse toLearningItem(VocabularyWord word, String studyMode, UserWordState state) {
        return new WordLearningItemResponse(
                word.getId(),
                word.getWord(),
                word.getTranslation(),
                word.getPhoneticUs(),
                word.getPhoneticUk(),
                word.getPartOfSpeech(),
                word.getExampleSentence(),
                word.getDifficulty(),
                studyMode,
                state == null ? "NEW" : state.getState(),
                state == null ? 0 : state.getMasteryLevel(),
                state == null ? null : state.getNextReviewAt());
    }

    private DailyStudySummaryResponse toDailySummaryResponse(DailyStudySummary summary) {
        return new DailyStudySummaryResponse(
                summary.getStudyDate(),
                summary.getNewWords(),
                summary.getReviewWords(),
                summary.getCorrectCount(),
                summary.getWrongCount(),
                summary.getStudySeconds(),
                summary.getScoreDelta(),
                summary.getCompletedTasks());
    }

    private String normalizeActivityType(String activityType) {
        String normalized = defaultValue(activityType, "REVIEW").toUpperCase();
        return switch (normalized) {
            case "WORD_NEW", "NEW_WORD", "LEARN" -> "NEW";
            case "WORD_REVIEW", "REVIEW_WORD", "LEARNING" -> "REVIEW";
            default -> normalized;
        };
    }

    private int normalizeQuality(Integer quality, Boolean correct) {
        if (quality != null) {
            return Math.max(0, Math.min(5, quality));
        }
        return Boolean.TRUE.equals(correct) ? 4 : 2;
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
