package com.englishlearner.server.dto.quiz;

import java.util.List;

public record QuizQuestionResponse(
        Long wordId,
        String word,
        List<String> options
) {
}
