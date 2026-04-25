package com.englishlearner.server.config;

import com.englishlearner.server.security.AuthenticatedUser;
import com.englishlearner.server.security.JwtService;
import com.englishlearner.server.service.AccessControlService;
import jakarta.servlet.FilterChain;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.http.HttpHeaders;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.web.authentication.WebAuthenticationDetailsSource;
import org.springframework.stereotype.Component;
import org.springframework.web.filter.OncePerRequestFilter;

import java.io.IOException;
import java.util.Optional;

@Component
public class JwtAuthenticationFilter extends OncePerRequestFilter {
    private final JwtService jwtService;
    private final AccessControlService accessControlService;

    public JwtAuthenticationFilter(JwtService jwtService,
                                   AccessControlService accessControlService) {
        this.jwtService = jwtService;
        this.accessControlService = accessControlService;
    }

    @Override
    protected void doFilterInternal(HttpServletRequest request,
                                    HttpServletResponse response,
                                    FilterChain filterChain) throws ServletException, IOException {
        String authHeader = request.getHeader(HttpHeaders.AUTHORIZATION);

        // 有效的 Bearer token 会被转换为 Spring Security 认证对象，供后续 Controller 使用。
        if (authHeader != null
                && authHeader.startsWith("Bearer ")
                && SecurityContextHolder.getContext().getAuthentication() == null) {
            String token = authHeader.substring(7);
            Optional<AuthenticatedUser> authUserOpt = jwtService.parseToken(token);

            if (authUserOpt.isPresent()) {
                AuthenticatedUser tokenUser = authUserOpt.get();
                Optional<AuthenticatedUser> currentUser = accessControlService.authenticate(tokenUser.userId());
                if (currentUser.isEmpty()) {
                    filterChain.doFilter(request, response);
                    return;
                }
                AuthenticatedUser authUser = currentUser.get();
                UsernamePasswordAuthenticationToken authenticationToken =
                        new UsernamePasswordAuthenticationToken(authUser, null, authUser.authorities());
                authenticationToken.setDetails(new WebAuthenticationDetailsSource().buildDetails(request));
                SecurityContextHolder.getContext().setAuthentication(authenticationToken);
            }
        }

        filterChain.doFilter(request, response);
    }
}
