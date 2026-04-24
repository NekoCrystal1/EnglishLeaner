package com.englishlearner.server.dto.book;

public record BookResponse(
        Long bookId,
        String title,
        String bookType,
        String sourceType,
        Long ownerUserId,
        String language,
        String level,
        String coverUrl,
        Integer itemCount,
        String contentStorageMode,
        String clientContentRef,
        String contentHash,
        String status
) {
}
