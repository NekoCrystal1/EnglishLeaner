package com.englishlearner.server.repository;

import com.englishlearner.server.model.UserAccount;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

import java.util.List;
import java.util.Optional;

public interface UserAccountRepository extends JpaRepository<UserAccount, Long> {
    Optional<UserAccount> findByUsernameAndDeletedFalse(String username);

    Optional<UserAccount> findByEmailAndDeletedFalse(String email);

    Optional<UserAccount> findByIdAndDeletedFalse(Long id);

    boolean existsByUsernameAndDeletedFalse(String username);

    boolean existsByEmailAndDeletedFalse(String email);

    @Query("select u from UserAccount u where u.deleted = false order by u.totalScore desc, u.id asc")
    List<UserAccount> findTopByScore(Pageable pageable);

    long countByDeletedFalse();

    long countByStatusAndDeletedFalse(String status);

    long countByRoleNameAndDeletedFalse(String roleName);

    @Query("""
            select u from UserAccount u
            where u.deleted = false
              and (:keyword is null
                   or lower(u.username) like lower(concat('%', :keyword, '%'))
                   or lower(coalesce(u.email, '')) like lower(concat('%', :keyword, '%'))
                   or lower(coalesce(u.displayName, '')) like lower(concat('%', :keyword, '%')))
              and (:status is null or u.status = :status)
              and (:roleName is null or u.roleName = :roleName)
            """)
    Page<UserAccount> searchAdminUsers(@Param("keyword") String keyword,
                                       @Param("status") String status,
                                       @Param("roleName") String roleName,
                                       Pageable pageable);
}
