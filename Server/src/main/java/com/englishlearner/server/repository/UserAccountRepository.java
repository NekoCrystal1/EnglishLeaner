package com.englishlearner.server.repository;

import com.englishlearner.server.model.UserAccount;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

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
}
