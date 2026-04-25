package com.englishlearner.server.dto.book;

public record SystemBookImportResult(
        String key,
        Long bookId,
        String title,
        String level,
        int wordCount,
        int relationCount,
        String contentHash
) {
}
