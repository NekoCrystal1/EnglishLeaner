package com.englishlearner.server.dto.word;

import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.Max;
import jakarta.validation.constraints.Min;
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

        @Min(value = 0, message = "quality must be between 0 and 5")
        @Max(value = 5, message = "quality must be between 0 and 5")
        Integer quality,

        Integer durationMs,

        @Size(max = 128, message = "clientEventId length must be <= 128")
        String clientEventId
) {
}
