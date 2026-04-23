package com.englishlearner.server.dto.auth;

public record UserProfileResponse(
        Long userId,
        String username,
        Integer totalScore,
        Integer studyDays
) {
}
