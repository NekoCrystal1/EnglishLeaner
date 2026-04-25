package com.englishlearner.server.repository;

import com.englishlearner.server.model.Book;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

import java.util.List;
import java.util.Optional;

public interface BookRepository extends JpaRepository<Book, Long> {
    Optional<Book> findByIdAndDeletedFalse(Long id);

    Optional<Book> findFirstByBookTypeAndStatusAndOwnerUserIdIsNullAndDeletedFalseOrderByIdAsc(String bookType, String status);

    Optional<Book> findFirstByTitleAndSourceTypeAndOwnerUserIdIsNullAndDeletedFalse(String title, String sourceType);

    @Query("""
            select b from Book b
            where b.deleted = false
              and b.status = 'ACTIVE'
              and (:bookType is null or b.bookType = :bookType)
              and (b.ownerUserId is null or b.ownerUserId = :userId)
            order by case when b.ownerUserId is null then 0 else 1 end, b.id asc
            """)
    List<Book> findVisibleBooks(@Param("userId") Long userId, @Param("bookType") String bookType);
}
