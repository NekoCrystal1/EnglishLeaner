package com.englishlearner.server.repository;

import com.englishlearner.server.model.AppRole;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;
import java.util.Optional;

public interface AppRoleRepository extends JpaRepository<AppRole, Long> {
    Optional<AppRole> findByCodeAndDeletedFalse(String code);

    List<AppRole> findAllByDeletedFalseOrderByCodeAsc();
}
