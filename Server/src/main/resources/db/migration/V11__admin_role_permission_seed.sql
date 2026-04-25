CREATE TABLE app_role (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    code NVARCHAR(32) NOT NULL,
    name NVARCHAR(64) NOT NULL,
    description NVARCHAR(255) NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT uk_app_role_code UNIQUE (code)
);

CREATE TABLE app_permission (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    code NVARCHAR(64) NOT NULL,
    name NVARCHAR(64) NOT NULL,
    module NVARCHAR(64) NOT NULL,
    description NVARCHAR(255) NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT uk_app_permission_code UNIQUE (code)
);

CREATE TABLE user_role (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    role_id BIGINT NOT NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_user_role_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_user_role_role FOREIGN KEY (role_id) REFERENCES app_role(id),
    CONSTRAINT uk_user_role_pair UNIQUE (user_id, role_id)
);

CREATE TABLE role_permission (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    role_id BIGINT NOT NULL,
    permission_id BIGINT NOT NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_role_permission_role FOREIGN KEY (role_id) REFERENCES app_role(id),
    CONSTRAINT fk_role_permission_permission FOREIGN KEY (permission_id) REFERENCES app_permission(id),
    CONSTRAINT uk_role_permission_pair UNIQUE (role_id, permission_id)
);

INSERT INTO app_role (code, name, description)
SELECT 'USER', N'普通用户', N'学习端默认角色'
WHERE NOT EXISTS (SELECT 1 FROM app_role WHERE code = 'USER');

INSERT INTO app_role (code, name, description)
SELECT 'ADMIN', N'系统管理员', N'后台管理角色'
WHERE NOT EXISTS (SELECT 1 FROM app_role WHERE code = 'ADMIN');

INSERT INTO app_permission (code, name, module, description)
SELECT 'LEARNING_ACCESS', N'学习端访问', N'learning', N'访问普通学习功能'
WHERE NOT EXISTS (SELECT 1 FROM app_permission WHERE code = 'LEARNING_ACCESS');

INSERT INTO app_permission (code, name, module, description)
SELECT 'ADMIN_ACCESS', N'后台访问', N'admin', N'访问后台管理入口'
WHERE NOT EXISTS (SELECT 1 FROM app_permission WHERE code = 'ADMIN_ACCESS');

INSERT INTO app_permission (code, name, module, description)
SELECT 'USER_READ', N'查看用户', N'admin', N'查看用户列表和概览'
WHERE NOT EXISTS (SELECT 1 FROM app_permission WHERE code = 'USER_READ');

INSERT INTO app_permission (code, name, module, description)
SELECT 'USER_WRITE', N'管理用户', N'admin', N'调整用户状态和角色'
WHERE NOT EXISTS (SELECT 1 FROM app_permission WHERE code = 'USER_WRITE');

INSERT INTO app_permission (code, name, module, description)
SELECT 'ROLE_READ', N'查看角色权限', N'admin', N'查看角色和权限配置'
WHERE NOT EXISTS (SELECT 1 FROM app_permission WHERE code = 'ROLE_READ');

INSERT INTO app_permission (code, name, module, description)
SELECT 'CONTENT_IMPORT', N'导入系统内容', N'content', N'导入系统词书等运营内容'
WHERE NOT EXISTS (SELECT 1 FROM app_permission WHERE code = 'CONTENT_IMPORT');

INSERT INTO role_permission (role_id, permission_id)
SELECT r.id, p.id
FROM app_role r
JOIN app_permission p ON p.code = 'LEARNING_ACCESS'
WHERE r.code = 'USER'
  AND NOT EXISTS (
      SELECT 1
      FROM role_permission rp
      WHERE rp.role_id = r.id
        AND rp.permission_id = p.id
  );

INSERT INTO role_permission (role_id, permission_id)
SELECT r.id, p.id
FROM app_role r
CROSS JOIN app_permission p
WHERE r.code = 'ADMIN'
  AND NOT EXISTS (
      SELECT 1
      FROM role_permission rp
      WHERE rp.role_id = r.id
        AND rp.permission_id = p.id
  );

INSERT INTO user_role (user_id, role_id)
SELECT u.id, r.id
FROM users u
JOIN app_role r ON r.code = CASE WHEN UPPER(u.role_name) = 'ADMIN' THEN 'ADMIN' ELSE 'USER' END
WHERE u.is_deleted = 0
  AND NOT EXISTS (
      SELECT 1
      FROM user_role ur
      WHERE ur.user_id = u.id
        AND ur.role_id = r.id
  );

CREATE INDEX idx_user_role_user ON user_role(user_id, is_deleted);
CREATE INDEX idx_user_role_role ON user_role(role_id, is_deleted);
CREATE INDEX idx_role_permission_role ON role_permission(role_id, is_deleted);
CREATE INDEX idx_role_permission_permission ON role_permission(permission_id, is_deleted);
