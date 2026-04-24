package com.englishlearner.server.dto.book;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;

public record CreateBookRequest(
        @NotBlank(message = "title is required")
        @Size(max = 200, message = "title length must be <= 200")
        String title,

        @NotBlank(message = "bookType is required")
        @Size(max = 32, message = "bookType length must be <= 32")
        String bookType,

        @Size(max = 32, message = "language length must be <= 32")
        String language,

        @Size(max = 32, message = "level length must be <= 32")
        String level,

        @Size(max = 512, message = "coverUrl length must be <= 512")
        String coverUrl,

        Integer itemCount,

        @Size(max = 512, message = "clientContentRef length must be <= 512")
        String clientContentRef,

        @Size(max = 128, message = "contentHash length must be <= 128")
        String contentHash
) {
}
