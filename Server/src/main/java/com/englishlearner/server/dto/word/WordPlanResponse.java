package com.englishlearner.server.dto.word;

import java.time.LocalDate;

public record WordPlanResponse(
        Long planId,
        Long bookId,
        String bookTitle,
        Integer dailyNewCount,
        Integer dailyReviewCount,
        Integer currentPosition,
        String reviewAlgorithm,
        LocalDate startedOn,
        LocalDate finishedOn,
        String status,
        Long learnedWords,
        Long masteredWords,
        Long dueReviewWords,
        Long totalWords
) {
}
