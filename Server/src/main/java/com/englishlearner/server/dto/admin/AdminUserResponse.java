package com.englishlearner.server.dto.admin;

import java.time.LocalDateTime;
import java.util.List;

public record AdminUserResponse(
        Long userId,
        String username,
        String email,
        String displayName,
        String status,
        String roleName,
        List<String> roles,
        Integer totalScore,
        Integer studyDays,
        LocalDateTime lastLoginAt,
        LocalDateTime createdAt
) {
}
