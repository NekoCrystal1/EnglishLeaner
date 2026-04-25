package com.englishlearner.server.dto.admin;

import java.util.List;

public record AdminUserPageResponse(
        List<AdminUserResponse> items,
        long total,
        int page,
        int size
) {
}
