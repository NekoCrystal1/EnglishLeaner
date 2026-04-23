package com.englishlearner.server.repository;

import com.englishlearner.server.model.StudyLog;
import org.springframework.data.jpa.repository.JpaRepository;

public interface StudyLogRepository extends JpaRepository<StudyLog, Long> {
}
