package com.englishlearner.server.repository;

import com.englishlearner.server.model.UserAuthIdentity;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface UserAuthIdentityRepository extends JpaRepository<UserAuthIdentity, Long> {
    Optional<UserAuthIdentity> findByProviderAndProviderUserIdAndDeletedFalse(String provider, String providerUserId);

    Optional<UserAuthIdentity> findByUserIdAndProviderAndDeletedFalse(Long userId, String provider);
}
