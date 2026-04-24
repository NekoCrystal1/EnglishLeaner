package com.englishlearner.server.repository;

import com.englishlearner.server.model.WordStudyEvent;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface WordStudyEventRepository extends JpaRepository<WordStudyEvent, Long> {
    boolean existsByUserIdAndClientEventIdAndDeletedFalse(Long userId, String clientEventId);

    Optional<WordStudyEvent> findByUserIdAndClientEventIdAndDeletedFalse(Long userId, String clientEventId);
}
