package com.englishlearner.server.dto.quiz;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;

public record SubmitAnswerRequest(
        @NotNull(message = "wordId 不能为空")
        Long wordId,

        @NotBlank(message = "selectedAnswer 不能为空")
        String selectedAnswer
) {
}
