package com.englishlearner.server.dto.admin;

public record AdminOverviewResponse(
        long totalUsers,
        long activeUsers,
        long disabledUsers,
        long adminUsers
) {
}
