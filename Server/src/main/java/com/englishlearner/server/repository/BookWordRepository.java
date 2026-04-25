package com.englishlearner.server.repository;

import com.englishlearner.server.model.BookWord;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

import java.util.List;
import java.util.Optional;

public interface BookWordRepository extends JpaRepository<BookWord, Long> {
    List<BookWord> findByBookIdAndDeletedFalseOrderBySortOrderAscIdAsc(Long bookId);

    @Query("""
            select bw from BookWord bw
            where bw.bookId = :bookId
              and bw.deleted = false
              and not exists (
                  select s.id from UserWordState s
                  where s.userId = :userId
                    and s.planId = :planId
                    and s.wordId = bw.wordId
                    and s.deleted = false
              )
            order by bw.sortOrder asc, bw.id asc
            """)
    List<BookWord> findUnseenWords(@Param("userId") Long userId,
                                   @Param("planId") Long planId,
                                   @Param("bookId") Long bookId,
                                   Pageable pageable);

    Optional<BookWord> findByBookIdAndWordIdAndDeletedFalse(Long bookId, Long wordId);

    long countByBookIdAndDeletedFalse(Long bookId);

    @Modifying
    @Query("delete from BookWord bw where bw.bookId = :bookId")
    int deleteAllByBookId(@Param("bookId") Long bookId);
}
