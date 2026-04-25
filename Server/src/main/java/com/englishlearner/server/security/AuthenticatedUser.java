package com.englishlearner.server.security;

import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;

import java.security.Principal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

public record AuthenticatedUser(
        Long userId,
        String username,
        Set<String> roles,
        Set<String> permissions
) implements Principal {
    public AuthenticatedUser(Long userId, String username) {
        this(userId, username, Set.of("USER"), Set.of());
    }

    public AuthenticatedUser {
        roles = normalize(roles);
        permissions = normalize(permissions);
    }

    @Override
    public String getName() {
        return username;
    }

    public Collection<? extends GrantedAuthority> authorities() {
        List<GrantedAuthority> authorities = new ArrayList<>();
        roles.forEach(role -> authorities.add(new SimpleGrantedAuthority("ROLE_" + role)));
        permissions.forEach(permission -> authorities.add(new SimpleGrantedAuthority(permission)));
        return List.copyOf(authorities);
    }

    public boolean hasRole(String role) {
        return roles.contains(normalizeValue(role));
    }

    public boolean hasPermission(String permission) {
        return permissions.contains(normalizeValue(permission));
    }

    private static Set<String> normalize(Set<String> values) {
        if (values == null || values.isEmpty()) {
            return Set.of();
        }
        TreeSet<String> normalized = new TreeSet<>();
        values.stream()
                .map(AuthenticatedUser::normalizeValue)
                .filter(value -> !value.isBlank())
                .forEach(normalized::add);
        return Set.copyOf(normalized);
    }

    private static String normalizeValue(String value) {
        return value == null ? "" : value.trim().toUpperCase();
    }
}
