package com.englishlearner.server.repository;

import com.englishlearner.server.model.UserWordPlan;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;
import java.util.Optional;

public interface UserWordPlanRepository extends JpaRepository<UserWordPlan, Long> {
    Optional<UserWordPlan> findFirstByUserIdAndStatusAndDeletedFalseOrderByIdDesc(Long userId, String status);

    Optional<UserWordPlan> findByIdAndUserIdAndDeletedFalse(Long id, Long userId);

    List<UserWordPlan> findByUserIdAndDeletedFalseOrderByCreatedAtDesc(Long userId);
}
