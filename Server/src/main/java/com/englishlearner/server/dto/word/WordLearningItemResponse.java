package com.englishlearner.server.dto.word;

import java.time.LocalDateTime;

public record WordLearningItemResponse(
        Long wordId,
        String word,
        String translation,
        String phoneticUs,
        String phoneticUk,
        String partOfSpeech,
        String exampleSentence,
        Integer difficulty,
        String studyMode,
        String state,
        Integer masteryLevel,
        LocalDateTime nextReviewAt
) {
}
