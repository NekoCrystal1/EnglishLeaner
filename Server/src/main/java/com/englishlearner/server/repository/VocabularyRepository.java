package com.englishlearner.server.repository;

import com.englishlearner.server.model.Vocabulary;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;
import java.util.Optional;

public interface VocabularyRepository extends JpaRepository<Vocabulary, Long> {
    long countByDeletedFalse();

    Page<Vocabulary> findByDeletedFalse(Pageable pageable);

    Optional<Vocabulary> findByIdAndDeletedFalse(Long id);

    List<Vocabulary> findByIdNotAndDeletedFalse(Long id);
}
