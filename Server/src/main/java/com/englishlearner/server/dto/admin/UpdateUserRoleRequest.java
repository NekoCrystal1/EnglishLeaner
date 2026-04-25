package com.englishlearner.server.dto.admin;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;

public record UpdateUserRoleRequest(
        @NotBlank(message = "roleCode is required")
        @Size(max = 32, message = "roleCode is too long")
        String roleCode
) {
}
