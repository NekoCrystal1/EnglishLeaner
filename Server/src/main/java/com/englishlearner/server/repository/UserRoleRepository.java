package com.englishlearner.server.repository;

import com.englishlearner.server.model.UserRole;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

import java.util.List;
import java.util.Optional;

public interface UserRoleRepository extends JpaRepository<UserRole, Long> {
    List<UserRole> findByUserId(Long userId);

    Optional<UserRole> findByUserIdAndRoleId(Long userId, Long roleId);

    @Query(value = """
            select r.code
            from user_role ur
            join app_role r on r.id = ur.role_id
            where ur.user_id = :userId
              and ur.is_deleted = 0
              and r.is_deleted = 0
            order by r.code
            """, nativeQuery = true)
    List<String> findRoleCodesByUserId(Long userId);
}
