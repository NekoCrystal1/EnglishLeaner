package com.englishlearner.server.dto.quiz;

public record SubmitAnswerResponse(
        boolean correct,
        String correctAnswer,
        int scoreDelta,
        int totalScore
) {
}
