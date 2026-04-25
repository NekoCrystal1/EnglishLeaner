package com.englishlearner.server.service;

import com.englishlearner.server.model.UserAccount;
import com.englishlearner.server.model.UserAuthIdentity;
import com.englishlearner.server.model.UserProfile;
import com.englishlearner.server.repository.UserAccountRepository;
import com.englishlearner.server.repository.UserAuthIdentityRepository;
import com.englishlearner.server.repository.UserProfileRepository;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.ApplicationArguments;
import org.springframework.boot.ApplicationRunner;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;

@Service
public class AdminBootstrapService implements ApplicationRunner {
    private static final String PASSWORD_PROVIDER = "PASSWORD";

    private final UserAccountRepository userAccountRepository;
    private final UserProfileRepository userProfileRepository;
    private final UserAuthIdentityRepository userAuthIdentityRepository;
    private final AccessControlService accessControlService;
    private final PasswordEncoder passwordEncoder;

    @Value("${app.admin.bootstrap.enabled:true}")
    private boolean enabled;

    @Value("${app.admin.bootstrap.username:admin}")
    private String username;

    @Value("${app.admin.bootstrap.password:Admin@123456}")
    private String password;

    @Value("${app.admin.bootstrap.display-name:系统管理员}")
    private String displayName;

    @Value("${app.admin.bootstrap.email:}")
    private String email;

    public AdminBootstrapService(UserAccountRepository userAccountRepository,
                                 UserProfileRepository userProfileRepository,
                                 UserAuthIdentityRepository userAuthIdentityRepository,
                                 AccessControlService accessControlService,
                                 PasswordEncoder passwordEncoder) {
        this.userAccountRepository = userAccountRepository;
        this.userProfileRepository = userProfileRepository;
        this.userAuthIdentityRepository = userAuthIdentityRepository;
        this.accessControlService = accessControlService;
        this.passwordEncoder = passwordEncoder;
    }

    @Override
    @Transactional
    public void run(ApplicationArguments args) {
        if (!enabled) {
            return;
        }

        String adminUsername = trimToDefault(username, "admin");
        UserAccount admin = userAccountRepository.findByUsernameAndDeletedFalse(adminUsername)
                .orElseGet(() -> createAdmin(adminUsername));

        accessControlService.assignSingleRole(admin, AccessControlService.ROLE_ADMIN);
        ensureProfile(admin);
        ensurePasswordIdentity(admin);
    }

    private UserAccount createAdmin(String adminUsername) {
        UserAccount admin = new UserAccount();
        admin.setUsername(adminUsername);
        admin.setPasswordHash(passwordEncoder.encode(trimToDefault(password, "Admin@123456")));
        admin.setDisplayName(trimToDefault(displayName, "系统管理员"));
        admin.setEmail(trimToNull(email));
        admin.setEmailVerified(admin.getEmail() != null);
        admin.setRoleName(AccessControlService.ROLE_ADMIN);
        admin.setStatus(AccessControlService.STATUS_ACTIVE);
        admin.setTotalScore(0);
        admin.setStudyDays(0);
        return userAccountRepository.save(admin);
    }

    private void ensureProfile(UserAccount admin) {
        if (userProfileRepository.existsByUserIdAndDeletedFalse(admin.getId())) {
            return;
        }
        UserProfile profile = new UserProfile();
        profile.setUserId(admin.getId());
        profile.setDailyMinutes(30);
        profile.setTimezone("Asia/Shanghai");
        userProfileRepository.save(profile);
    }

    private void ensurePasswordIdentity(UserAccount admin) {
        UserAuthIdentity identity = userAuthIdentityRepository
                .findByUserIdAndProviderAndDeletedFalse(admin.getId(), PASSWORD_PROVIDER)
                .orElseGet(() -> {
                    UserAuthIdentity created = new UserAuthIdentity();
                    created.setUserId(admin.getId());
                    created.setProvider(PASSWORD_PROVIDER);
                    created.setBoundAt(LocalDateTime.now());
                    return created;
                });
        identity.setProviderUserId(admin.getUsername());
        identity.setCredentialHash(admin.getPasswordHash());
        userAuthIdentityRepository.save(identity);
    }

    private String trimToDefault(String value, String defaultValue) {
        String trimmed = trimToNull(value);
        return trimmed == null ? defaultValue : trimmed;
    }

    private String trimToNull(String value) {
        if (value == null) {
            return null;
        }
        String trimmed = value.trim();
        return trimmed.isEmpty() ? null : trimmed;
    }
}
