package com.englishlearner.server.dto.auth;

public record UserProfileResponse(
        Long userId,
        String username,
        String email,
        Boolean emailVerified,
        String displayName,
        String avatarUrl,
        String status,
        Integer totalScore,
        Integer studyDays
) {
}
