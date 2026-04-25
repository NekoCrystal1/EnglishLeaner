package com.englishlearner.server.service;

import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.auth.LoginRequest;
import com.englishlearner.server.dto.auth.LoginResponse;
import com.englishlearner.server.dto.auth.RegisterRequest;
import com.englishlearner.server.dto.auth.UserProfileResponse;
import com.englishlearner.server.model.UserAuthIdentity;
import com.englishlearner.server.model.UserAccount;
import com.englishlearner.server.model.UserProfile;
import com.englishlearner.server.repository.UserAuthIdentityRepository;
import com.englishlearner.server.repository.UserAccountRepository;
import com.englishlearner.server.repository.UserProfileRepository;
import com.englishlearner.server.security.JwtService;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;

@Service
public class AuthService {
    private static final String PASSWORD_PROVIDER = "PASSWORD";

    private final UserAccountRepository userAccountRepository;
    private final UserProfileRepository userProfileRepository;
    private final UserAuthIdentityRepository userAuthIdentityRepository;
    private final PasswordEncoder passwordEncoder;
    private final JwtService jwtService;

    public AuthService(UserAccountRepository userAccountRepository,
                       UserProfileRepository userProfileRepository,
                       UserAuthIdentityRepository userAuthIdentityRepository,
                       PasswordEncoder passwordEncoder,
                       JwtService jwtService) {
        this.userAccountRepository = userAccountRepository;
        this.userProfileRepository = userProfileRepository;
        this.userAuthIdentityRepository = userAuthIdentityRepository;
        this.passwordEncoder = passwordEncoder;
        this.jwtService = jwtService;
    }

    @Transactional
    public UserProfileResponse register(RegisterRequest request) {
        // 注册时同时创建账号、默认资料和密码登录身份。
        String username = request.username().trim();
        if (userAccountRepository.existsByUsernameAndDeletedFalse(username)) {
            throw BusinessException.badRequest("account already exists");
        }
        String email = trimToNull(request.email());
        if (email != null && userAccountRepository.existsByEmailAndDeletedFalse(email)) {
            throw BusinessException.badRequest("email already exists");
        }

        UserAccount user = new UserAccount();
        user.setUsername(username);
        user.setPasswordHash(passwordEncoder.encode(request.password()));
        user.setEmail(email);
        user.setDisplayName(trimToNull(request.displayName()));
        user.setTotalScore(0);
        user.setStudyDays(0);
        user.setRoleName("USER");
        user.setStatus("ACTIVE");

        UserAccount saved = userAccountRepository.save(user);
        createDefaultProfile(saved.getId());
        createPasswordIdentity(saved);
        return toProfile(saved);
    }

    @Transactional
    public LoginResponse loginWithPassword(LoginRequest request) {
        // 登录兼容新的 account 字段和旧的 username 字段。
        String account = firstNotBlank(request.account(), request.username());
        if (account == null) {
            throw BusinessException.badRequest("username or account is required");
        }

        UserAccount user = findLoginAccount(account)
                .orElseThrow(() -> BusinessException.unauthorized("invalid username or password"));

        if (!passwordEncoder.matches(request.password(), user.getPasswordHash())) {
            throw BusinessException.unauthorized("invalid username or password");
        }

        LocalDateTime loginAt = LocalDateTime.now();
        user.setLastLoginAt(loginAt);
        userAccountRepository.save(user);
        touchPasswordIdentity(user, loginAt);

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
                user.getEmail(),
                user.getEmailVerified(),
                user.getDisplayName(),
                user.getAvatarUrl(),
                user.getStatus(),
                user.getTotalScore(),
                user.getStudyDays()
        );
    }

    private java.util.Optional<UserAccount> findLoginAccount(String account) {
        java.util.Optional<UserAccount> byUsername = userAccountRepository.findByUsernameAndDeletedFalse(account);
        if (byUsername.isPresent()) {
            return byUsername;
        }
        return userAccountRepository.findByEmailAndDeletedFalse(account);
    }

    private void createDefaultProfile(Long userId) {
        UserProfile profile = new UserProfile();
        profile.setUserId(userId);
        profile.setDailyMinutes(30);
        profile.setTimezone("Asia/Shanghai");
        userProfileRepository.save(profile);
    }

    private void createPasswordIdentity(UserAccount user) {
        UserAuthIdentity identity = new UserAuthIdentity();
        identity.setUserId(user.getId());
        identity.setProvider(PASSWORD_PROVIDER);
        identity.setProviderUserId(user.getUsername());
        identity.setCredentialHash(user.getPasswordHash());
        identity.setBoundAt(LocalDateTime.now());
        userAuthIdentityRepository.save(identity);
    }

    private void touchPasswordIdentity(UserAccount user, LocalDateTime loginAt) {
        UserAuthIdentity identity = userAuthIdentityRepository
                .findByUserIdAndProviderAndDeletedFalse(user.getId(), PASSWORD_PROVIDER)
                .orElseGet(() -> {
                    UserAuthIdentity created = new UserAuthIdentity();
                    created.setUserId(user.getId());
                    created.setProvider(PASSWORD_PROVIDER);
                    created.setProviderUserId(user.getUsername());
                    created.setCredentialHash(user.getPasswordHash());
                    created.setBoundAt(loginAt);
                    return created;
                });
        identity.setProviderUserId(user.getUsername());
        identity.setCredentialHash(user.getPasswordHash());
        identity.setLastLoginAt(loginAt);
        userAuthIdentityRepository.save(identity);
    }

    private String firstNotBlank(String first, String second) {
        String value = trimToNull(first);
        return value != null ? value : trimToNull(second);
    }

    private String trimToNull(String value) {
        if (value == null) {
            return null;
        }
        String trimmed = value.trim();
        return trimmed.isEmpty() ? null : trimmed;
    }
}
