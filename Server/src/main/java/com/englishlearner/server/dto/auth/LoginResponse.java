package com.englishlearner.server.dto.auth;

public record LoginResponse(
        String token,
        UserProfileResponse user
) {
}
