package com.englishlearner.server.repository;

import com.englishlearner.server.model.VocabularyWord;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Collection;
import java.util.List;
import java.util.Optional;

public interface VocabularyWordRepository extends JpaRepository<VocabularyWord, Long> {
    long countByDeletedFalse();

    Page<VocabularyWord> findByDeletedFalse(Pageable pageable);

    Optional<VocabularyWord> findByIdAndDeletedFalse(Long id);

    List<VocabularyWord> findByIdNotAndDeletedFalse(Long id);

    List<VocabularyWord> findByIdInAndDeletedFalse(Collection<Long> ids);

    List<VocabularyWord> findByNormalizedWordInAndDeletedFalse(Collection<String> normalizedWords);
}
