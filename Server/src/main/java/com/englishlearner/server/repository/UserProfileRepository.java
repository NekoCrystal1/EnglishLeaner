package com.englishlearner.server.repository;

import com.englishlearner.server.model.UserProfile;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface UserProfileRepository extends JpaRepository<UserProfile, Long> {
    Optional<UserProfile> findByUserIdAndDeletedFalse(Long userId);

    boolean existsByUserIdAndDeletedFalse(Long userId);
}
