package com.englishlearner.server.service;

import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.model.AppRole;
import com.englishlearner.server.model.UserAccount;
import com.englishlearner.server.model.UserRole;
import com.englishlearner.server.repository.AppRoleRepository;
import com.englishlearner.server.repository.RolePermissionRepository;
import com.englishlearner.server.repository.UserAccountRepository;
import com.englishlearner.server.repository.UserRoleRepository;
import com.englishlearner.server.security.AuthenticatedUser;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Locale;
import java.util.Optional;
import java.util.Set;

@Service
public class AccessControlService {
    public static final String ROLE_USER = "USER";
    public static final String ROLE_ADMIN = "ADMIN";
    public static final String STATUS_ACTIVE = "ACTIVE";

    private static final List<String> DEFAULT_USER_PERMISSIONS = List.of("LEARNING_ACCESS");
    private static final List<String> DEFAULT_ADMIN_PERMISSIONS = List.of(
            "ADMIN_ACCESS",
            "CONTENT_IMPORT",
            "LEARNING_ACCESS",
            "ROLE_READ",
            "USER_READ",
            "USER_WRITE"
    );

    private final UserAccountRepository userAccountRepository;
    private final AppRoleRepository appRoleRepository;
    private final UserRoleRepository userRoleRepository;
    private final RolePermissionRepository rolePermissionRepository;

    public AccessControlService(UserAccountRepository userAccountRepository,
                                AppRoleRepository appRoleRepository,
                                UserRoleRepository userRoleRepository,
                                RolePermissionRepository rolePermissionRepository) {
        this.userAccountRepository = userAccountRepository;
        this.appRoleRepository = appRoleRepository;
        this.userRoleRepository = userRoleRepository;
        this.rolePermissionRepository = rolePermissionRepository;
    }

    @Transactional(readOnly = true)
    public Optional<AuthenticatedUser> authenticate(Long userId) {
        return userAccountRepository.findByIdAndDeletedFalse(userId)
                .filter(user -> STATUS_ACTIVE.equals(normalize(user.getStatus())))
                .map(user -> {
                    UserAccess access = resolveAccess(user);
                    return new AuthenticatedUser(
                            user.getId(),
                            user.getUsername(),
                            Set.copyOf(access.roles()),
                            Set.copyOf(access.permissions())
                    );
                });
    }

    @Transactional(readOnly = true)
    public UserAccess resolveAccess(UserAccount user) {
        List<String> roleCodes = new ArrayList<>(userRoleRepository.findRoleCodesByUserId(user.getId()));
        String fallbackRole = normalizeRoleOrDefault(user.getRoleName());
        if (roleCodes.stream().noneMatch(role -> normalize(role).equals(fallbackRole))) {
            roleCodes.add(fallbackRole);
        }

        List<String> roles = normalizeDistinct(roleCodes);
        List<String> permissions = roles.isEmpty()
                ? List.of()
                : normalizeDistinct(rolePermissionRepository.findPermissionCodesByRoleCodes(roles));

        if (permissions.isEmpty()) {
            permissions = fallbackPermissions(roles);
        }

        String primaryRole = roles.contains(ROLE_ADMIN) ? ROLE_ADMIN : (roles.isEmpty() ? ROLE_USER : roles.get(0));
        return new UserAccess(primaryRole, roles, permissions);
    }

    @Transactional
    public void assignSingleRole(UserAccount user, String roleCode) {
        String normalizedRoleCode = normalizeRoleOrDefault(roleCode);
        AppRole role = appRoleRepository.findByCodeAndDeletedFalse(normalizedRoleCode)
                .orElseThrow(() -> BusinessException.badRequest("role does not exist"));

        boolean hasTargetRole = false;
        for (UserRole userRole : userRoleRepository.findByUserId(user.getId())) {
            if (role.getId().equals(userRole.getRoleId())) {
                userRole.setDeleted(false);
                hasTargetRole = true;
            } else {
                userRole.setDeleted(true);
            }
            userRoleRepository.save(userRole);
        }

        if (!hasTargetRole) {
            UserRole created = new UserRole();
            created.setUserId(user.getId());
            created.setRoleId(role.getId());
            userRoleRepository.save(created);
        }

        user.setRoleName(normalizedRoleCode);
        userAccountRepository.save(user);
    }

    public String normalizeRoleOrDefault(String roleCode) {
        String normalized = normalize(roleCode);
        return normalized.isBlank() ? ROLE_USER : normalized;
    }

    private List<String> fallbackPermissions(Collection<String> roles) {
        if (roles.contains(ROLE_ADMIN)) {
            return DEFAULT_ADMIN_PERMISSIONS;
        }
        if (roles.contains(ROLE_USER)) {
            return DEFAULT_USER_PERMISSIONS;
        }
        return List.of();
    }

    private List<String> normalizeDistinct(Collection<String> values) {
        LinkedHashSet<String> normalized = new LinkedHashSet<>();
        values.stream()
                .map(this::normalize)
                .filter(value -> !value.isBlank())
                .forEach(normalized::add);
        return List.copyOf(normalized);
    }

    private String normalize(String value) {
        return value == null ? "" : value.trim().toUpperCase(Locale.ROOT);
    }

    public record UserAccess(
            String primaryRole,
            List<String> roles,
            List<String> permissions
    ) {
    }
}
