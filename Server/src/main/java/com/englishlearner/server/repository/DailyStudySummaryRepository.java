package com.englishlearner.server.repository;

import com.englishlearner.server.model.DailyStudySummary;
import org.springframework.data.jpa.repository.JpaRepository;

import java.time.LocalDate;
import java.util.Optional;

public interface DailyStudySummaryRepository extends JpaRepository<DailyStudySummary, Long> {
    Optional<DailyStudySummary> findByUserIdAndStudyDateAndDeletedFalse(Long userId, LocalDate studyDate);
}
