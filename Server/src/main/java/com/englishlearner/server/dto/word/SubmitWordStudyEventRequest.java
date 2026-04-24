package com.englishlearner.server.dto.word;

import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.Size;

public record SubmitWordStudyEventRequest(
        Long planId,

        @NotNull(message = "wordId is required")
        Long wordId,

        @Size(max = 32, message = "activityType length must be <= 32")
        String activityType,

        @Size(max = 256, message = "selectedAnswer length must be <= 256")
        String selectedAnswer,

        @NotNull(message = "correct is required")
        Boolean correct,

        Integer durationMs,

        @Size(max = 128, message = "clientEventId length must be <= 128")
        String clientEventId
) {
}
