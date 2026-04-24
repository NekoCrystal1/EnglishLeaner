package com.englishlearner.server.dto.word;

import jakarta.validation.constraints.Max;
import jakarta.validation.constraints.Min;

public record UpdateWordPlanRequest(
        @Min(value = 1, message = "dailyNewCount must be >= 1")
        @Max(value = 200, message = "dailyNewCount must be <= 200")
        Integer dailyNewCount,

        @Min(value = 1, message = "dailyReviewCount must be >= 1")
        @Max(value = 500, message = "dailyReviewCount must be <= 500")
        Integer dailyReviewCount,

        String status
) {
}
