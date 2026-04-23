package com.englishlearner.server.dto.auth;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;

public record RegisterRequest(
        @NotBlank(message = "username is required")
        @Size(min = 2, max = 64, message = "username length must be in 2..64")
        String username,

        @NotBlank(message = "password is required")
        @Size(min = 6, max = 64, message = "password length must be in 6..64")
        String password
) {
}
