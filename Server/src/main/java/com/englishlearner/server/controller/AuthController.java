package com.englishlearner.server.controller;

import com.englishlearner.server.common.ApiResponse;
import com.englishlearner.server.dto.auth.LoginRequest;
import com.englishlearner.server.dto.auth.LoginResponse;
import com.englishlearner.server.dto.auth.RegisterRequest;
import com.englishlearner.server.dto.auth.UserProfileResponse;
import com.englishlearner.server.service.AuthService;
import jakarta.validation.Valid;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api/v1/auth")
public class AuthController {
    private final AuthService authService;

    public AuthController(AuthService authService) {
        this.authService = authService;
    }

    @PostMapping("/register")
    public ApiResponse<UserProfileResponse> register(@Valid @RequestBody RegisterRequest request) {
        return ApiResponse.ok("register success", authService.register(request));
    }

    @PostMapping("/login")
    public ApiResponse<LoginResponse> login(@Valid @RequestBody LoginRequest request) {
        return ApiResponse.ok("login success", authService.loginWithPassword(request));
    }

    @PostMapping("/login/password")
    public ApiResponse<LoginResponse> loginWithPassword(@Valid @RequestBody LoginRequest request) {
        return ApiResponse.ok("login success", authService.loginWithPassword(request));
    }
}
