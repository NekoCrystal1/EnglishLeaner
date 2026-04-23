package com.englishlearner.server.service;

import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.auth.LoginRequest;
import com.englishlearner.server.dto.auth.LoginResponse;
import com.englishlearner.server.dto.auth.RegisterRequest;
import com.englishlearner.server.dto.auth.UserProfileResponse;
import com.englishlearner.server.model.UserAccount;
import com.englishlearner.server.repository.UserAccountRepository;
import com.englishlearner.server.security.JwtService;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;

@Service
public class AuthService {
    private final UserAccountRepository userAccountRepository;
    private final PasswordEncoder passwordEncoder;
    private final JwtService jwtService;

    public AuthService(UserAccountRepository userAccountRepository,
                       PasswordEncoder passwordEncoder,
                       JwtService jwtService) {
        this.userAccountRepository = userAccountRepository;
        this.passwordEncoder = passwordEncoder;
        this.jwtService = jwtService;
    }

    public UserProfileResponse register(RegisterRequest request) {
        String username = request.username().trim();
        if (userAccountRepository.existsByUsernameAndDeletedFalse(username)) {
            throw BusinessException.badRequest("account already exists");
        }

        UserAccount user = new UserAccount();
        user.setUsername(username);
        user.setPasswordHash(passwordEncoder.encode(request.password()));
        user.setTotalScore(0);
        user.setStudyDays(0);
        user.setRoleName("USER");

        UserAccount saved = userAccountRepository.save(user);
        return toProfile(saved);
    }

    public LoginResponse login(LoginRequest request) {
        String username = request.username().trim();
        UserAccount user = userAccountRepository.findByUsernameAndDeletedFalse(username)
                .orElseThrow(() -> BusinessException.unauthorized("invalid username or password"));

        if (!passwordEncoder.matches(request.password(), user.getPasswordHash())) {
            throw BusinessException.unauthorized("invalid username or password");
        }

        String token = jwtService.generateToken(user.getId(), user.getUsername());
        return new LoginResponse(token, toProfile(user));
    }

    public UserProfileResponse getCurrentUser(Long userId) {
        UserAccount user = userAccountRepository.findByIdAndDeletedFalse(userId)
                .orElseThrow(() -> BusinessException.unauthorized("user not found or session expired"));
        return toProfile(user);
    }

    private UserProfileResponse toProfile(UserAccount user) {
        return new UserProfileResponse(
                user.getId(),
                user.getUsername(),
                user.getTotalScore(),
                user.getStudyDays()
        );
    }
}
