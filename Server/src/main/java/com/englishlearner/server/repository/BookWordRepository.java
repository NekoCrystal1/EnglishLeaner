package com.englishlearner.server.repository;

import com.englishlearner.server.model.BookWord;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;
import java.util.Optional;

public interface BookWordRepository extends JpaRepository<BookWord, Long> {
    List<BookWord> findByBookIdAndDeletedFalseOrderBySortOrderAscIdAsc(Long bookId);

    List<BookWord> findByBookIdAndSortOrderGreaterThanAndDeletedFalseOrderBySortOrderAscIdAsc(
            Long bookId,
            Integer sortOrder,
            Pageable pageable);

    Optional<BookWord> findByBookIdAndWordIdAndDeletedFalse(Long bookId, Long wordId);

    long countByBookIdAndDeletedFalse(Long bookId);
}
