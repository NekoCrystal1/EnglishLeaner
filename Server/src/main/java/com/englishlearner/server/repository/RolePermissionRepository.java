package com.englishlearner.server.repository;

import com.englishlearner.server.model.RolePermission;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

import java.util.Collection;
import java.util.List;

public interface RolePermissionRepository extends JpaRepository<RolePermission, Long> {
    @Query(value = """
            select distinct p.code
            from role_permission rp
            join app_role r on r.id = rp.role_id
            join app_permission p on p.id = rp.permission_id
            where r.code in (:roleCodes)
              and rp.is_deleted = 0
              and r.is_deleted = 0
              and p.is_deleted = 0
            order by p.code
            """, nativeQuery = true)
    List<String> findPermissionCodesByRoleCodes(Collection<String> roleCodes);

    @Query(value = """
            select distinct p.code
            from role_permission rp
            join app_permission p on p.id = rp.permission_id
            where rp.role_id = :roleId
              and rp.is_deleted = 0
              and p.is_deleted = 0
            order by p.code
            """, nativeQuery = true)
    List<String> findPermissionCodesByRoleId(Long roleId);
}
