package com.englishlearner.server.dto.ranking;

public record RankingItemResponse(
        int rank,
        Long userId,
        String username,
        int totalScore
) {
}
