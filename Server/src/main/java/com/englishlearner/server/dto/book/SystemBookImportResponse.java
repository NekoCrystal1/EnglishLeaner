package com.englishlearner.server.dto.book;

import java.util.List;

public record SystemBookImportResponse(
        int bookCount,
        int wordCount,
        int relationCount,
        List<SystemBookImportResult> books
) {
}
