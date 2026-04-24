package com.englishlearner.server.repository;

import com.englishlearner.server.model.UserWordState;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

public interface UserWordStateRepository extends JpaRepository<UserWordState, Long> {
    Optional<UserWordState> findByUserIdAndPlanIdAndWordIdAndDeletedFalse(Long userId, Long planId, Long wordId);

    long countByUserIdAndPlanIdAndDeletedFalse(Long userId, Long planId);

    long countByUserIdAndPlanIdAndStateAndDeletedFalse(Long userId, Long planId, String state);

    @Query("""
            select count(s) from UserWordState s
            where s.userId = :userId
              and s.planId = :planId
              and s.deleted = false
              and s.nextReviewAt is not null
              and s.nextReviewAt <= :now
            """)
    long countDueReviewStates(@Param("userId") Long userId,
                              @Param("planId") Long planId,
                              @Param("now") LocalDateTime now);

    @Query("""
            select s from UserWordState s
            where s.userId = :userId
              and s.planId = :planId
              and s.deleted = false
              and s.nextReviewAt is not null
              and s.nextReviewAt <= :now
            order by s.nextReviewAt asc, s.id asc
            """)
    List<UserWordState> findDueReviewStates(@Param("userId") Long userId,
                                            @Param("planId") Long planId,
                                            @Param("now") LocalDateTime now,
                                            Pageable pageable);
}
