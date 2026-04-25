package com.englishlearner.server.controller;

import com.englishlearner.server.common.ApiResponse;
import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.admin.AdminOverviewResponse;
import com.englishlearner.server.dto.admin.AdminUserPageResponse;
import com.englishlearner.server.dto.admin.AdminUserResponse;
import com.englishlearner.server.dto.admin.RoleResponse;
import com.englishlearner.server.dto.admin.UpdateUserRoleRequest;
import com.englishlearner.server.dto.admin.UpdateUserStatusRequest;
import com.englishlearner.server.security.AuthenticatedUser;
import com.englishlearner.server.service.AdminUserService;
import jakarta.validation.Valid;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.security.core.Authentication;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/api/v1/admin")
@PreAuthorize("hasAuthority('ADMIN_ACCESS')")
public class AdminController {
    private final AdminUserService adminUserService;

    public AdminController(AdminUserService adminUserService) {
        this.adminUserService = adminUserService;
    }

    @GetMapping("/overview")
    @PreAuthorize("hasAuthority('USER_READ')")
    public ApiResponse<AdminOverviewResponse> overview() {
        return ApiResponse.ok(adminUserService.overview());
    }

    @GetMapping("/users")
    @PreAuthorize("hasAuthority('USER_READ')")
    public ApiResponse<AdminUserPageResponse> users(@RequestParam(required = false) String keyword,
                                                    @RequestParam(required = false) String status,
                                                    @RequestParam(required = false) String roleName,
                                                    @RequestParam(defaultValue = "0") int page,
                                                    @RequestParam(defaultValue = "20") int size) {
        return ApiResponse.ok(adminUserService.searchUsers(keyword, status, roleName, page, size));
    }

    @PutMapping("/users/{userId}/status")
    @PreAuthorize("hasAuthority('USER_WRITE')")
    public ApiResponse<AdminUserResponse> updateStatus(@PathVariable Long userId,
                                                       @Valid @RequestBody UpdateUserStatusRequest request,
                                                       Authentication authentication) {
        return ApiResponse.ok("status updated",
                adminUserService.updateStatus(currentUserId(authentication), userId, request.status()));
    }

    @PutMapping("/users/{userId}/role")
    @PreAuthorize("hasAuthority('USER_WRITE')")
    public ApiResponse<AdminUserResponse> updateRole(@PathVariable Long userId,
                                                     @Valid @RequestBody UpdateUserRoleRequest request,
                                                     Authentication authentication) {
        return ApiResponse.ok("role updated",
                adminUserService.updateRole(currentUserId(authentication), userId, request.roleCode()));
    }

    @GetMapping("/roles")
    @PreAuthorize("hasAuthority('ROLE_READ')")
    public ApiResponse<List<RoleResponse>> roles() {
        return ApiResponse.ok(adminUserService.listRoles());
    }

    private Long currentUserId(Authentication authentication) {
        if (authentication == null || !(authentication.getPrincipal() instanceof AuthenticatedUser authUser)) {
            throw BusinessException.unauthorized("please login first");
        }
        return authUser.userId();
    }
}
