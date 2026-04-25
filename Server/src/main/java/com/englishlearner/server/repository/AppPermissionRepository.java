package com.englishlearner.server.repository;

import com.englishlearner.server.model.AppPermission;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface AppPermissionRepository extends JpaRepository<AppPermission, Long> {
    List<AppPermission> findAllByDeletedFalseOrderByModuleAscCodeAsc();
}
