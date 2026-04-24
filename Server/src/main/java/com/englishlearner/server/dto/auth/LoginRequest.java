package com.englishlearner.server.dto.auth;

import jakarta.validation.constraints.NotBlank;

public record LoginRequest(
        String username,

        String account,

        @NotBlank(message = "password is required")
        String password
) {
}
