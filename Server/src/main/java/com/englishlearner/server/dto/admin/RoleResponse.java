package com.englishlearner.server.dto.admin;

import java.util.List;

public record RoleResponse(
        String code,
        String name,
        String description,
        List<String> permissions
) {
}
