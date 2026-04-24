package com.englishlearner.server.repository;

import com.englishlearner.server.model.ScoreTransaction;
import org.springframework.data.jpa.repository.JpaRepository;

public interface ScoreTransactionRepository extends JpaRepository<ScoreTransaction, Long> {
}
