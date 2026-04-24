package com.englishlearner.server.dto.word;

import java.time.LocalDateTime;

public record WordStudyEventResponse(
        Long eventId,
        Long planId,
        Long wordId,
        String state,
        Integer masteryLevel,
        Integer scoreDelta,
        Integer totalScore,
        LocalDateTime nextReviewAt
) {
}
