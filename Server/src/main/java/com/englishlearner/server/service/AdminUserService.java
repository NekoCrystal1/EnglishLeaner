package com.englishlearner.server.service;

import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.admin.AdminOverviewResponse;
import com.englishlearner.server.dto.admin.AdminUserPageResponse;
import com.englishlearner.server.dto.admin.AdminUserResponse;
import com.englishlearner.server.dto.admin.RoleResponse;
import com.englishlearner.server.model.AppRole;
import com.englishlearner.server.model.UserAccount;
import com.englishlearner.server.repository.AppRoleRepository;
import com.englishlearner.server.repository.RolePermissionRepository;
import com.englishlearner.server.repository.UserAccountRepository;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Sort;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.Locale;

@Service
public class AdminUserService {
    private final UserAccountRepository userAccountRepository;
    private final AppRoleRepository appRoleRepository;
    private final RolePermissionRepository rolePermissionRepository;
    private final AccessControlService accessControlService;

    public AdminUserService(UserAccountRepository userAccountRepository,
                            AppRoleRepository appRoleRepository,
                            RolePermissionRepository rolePermissionRepository,
                            AccessControlService accessControlService) {
        this.userAccountRepository = userAccountRepository;
        this.appRoleRepository = appRoleRepository;
        this.rolePermissionRepository = rolePermissionRepository;
        this.accessControlService = accessControlService;
    }

    @Transactional(readOnly = true)
    public AdminOverviewResponse overview() {
        return new AdminOverviewResponse(
                userAccountRepository.countByDeletedFalse(),
                userAccountRepository.countByStatusAndDeletedFalse(AccessControlService.STATUS_ACTIVE),
                userAccountRepository.countByStatusAndDeletedFalse("DISABLED"),
                userAccountRepository.countByRoleNameAndDeletedFalse(AccessControlService.ROLE_ADMIN)
        );
    }

    @Transactional(readOnly = true)
    public AdminUserPageResponse searchUsers(String keyword, String status, String roleName, int page, int size) {
        int safePage = Math.max(page, 0);
        int safeSize = Math.min(Math.max(size, 1), 100);
        PageRequest pageable = PageRequest.of(safePage, safeSize, Sort.by(Sort.Direction.DESC, "id"));
        Page<UserAccount> users = userAccountRepository.searchAdminUsers(
                trimToNull(keyword),
                normalizeOptional(status),
                normalizeOptional(roleName),
                pageable
        );

        return new AdminUserPageResponse(
                users.getContent().stream().map(this::toUserResponse).toList(),
                users.getTotalElements(),
                safePage,
                safeSize
        );
    }

    @Transactional(readOnly = true)
    public List<RoleResponse> listRoles() {
        return appRoleRepository.findAllByDeletedFalseOrderByCodeAsc().stream()
                .map(role -> new RoleResponse(
                        role.getCode(),
                        role.getName(),
                        role.getDescription(),
                        rolePermissionRepository.findPermissionCodesByRoleId(role.getId())
                ))
                .toList();
    }

    @Transactional
    public AdminUserResponse updateStatus(Long actorUserId, Long targetUserId, String status) {
        UserAccount target = findUser(targetUserId);
        String normalizedStatus = normalizeOptional(status);
        if (!AccessControlService.STATUS_ACTIVE.equals(normalizedStatus) && !"DISABLED".equals(normalizedStatus)) {
            throw BusinessException.badRequest("status must be ACTIVE or DISABLED");
        }
        if (target.getId().equals(actorUserId) && !"ACTIVE".equals(normalizedStatus)) {
            throw BusinessException.badRequest("cannot disable current admin account");
        }
        target.setStatus(normalizedStatus);
        return toUserResponse(userAccountRepository.save(target));
    }

    @Transactional
    public AdminUserResponse updateRole(Long actorUserId, Long targetUserId, String roleCode) {
        UserAccount target = findUser(targetUserId);
        String normalizedRole = accessControlService.normalizeRoleOrDefault(roleCode);
        if (target.getId().equals(actorUserId) && !AccessControlService.ROLE_ADMIN.equals(normalizedRole)) {
            throw BusinessException.badRequest("cannot remove current admin role");
        }
        accessControlService.assignSingleRole(target, normalizedRole);
        return toUserResponse(target);
    }

    private UserAccount findUser(Long userId) {
        return userAccountRepository.findByIdAndDeletedFalse(userId)
                .orElseThrow(() -> BusinessException.notFound("user not found"));
    }

    private AdminUserResponse toUserResponse(UserAccount user) {
        AccessControlService.UserAccess access = accessControlService.resolveAccess(user);
        return new AdminUserResponse(
                user.getId(),
                user.getUsername(),
                user.getEmail(),
                user.getDisplayName(),
                user.getStatus(),
                access.primaryRole(),
                access.roles(),
                user.getTotalScore(),
                user.getStudyDays(),
                user.getLastLoginAt(),
                user.getCreatedAt()
        );
    }

    private String normalizeOptional(String value) {
        String trimmed = trimToNull(value);
        return trimmed == null ? null : trimmed.toUpperCase(Locale.ROOT);
    }

    private String trimToNull(String value) {
        if (value == null) {
            return null;
        }
        String trimmed = value.trim();
        return trimmed.isEmpty() ? null : trimmed;
    }
}
