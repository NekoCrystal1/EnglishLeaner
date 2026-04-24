ALTER TABLE users ADD email NVARCHAR(128) NULL;
ALTER TABLE users ADD email_verified BIT NOT NULL DEFAULT 0;
ALTER TABLE users ADD phone NVARCHAR(32) NULL;
ALTER TABLE users ADD display_name NVARCHAR(64) NULL;
ALTER TABLE users ADD avatar_url NVARCHAR(512) NULL;
ALTER TABLE users ADD status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE';
ALTER TABLE users ADD last_study_date DATE NULL;
ALTER TABLE users ADD last_login_at DATETIME2 NULL;

CREATE TABLE user_auth_identity (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    provider NVARCHAR(32) NOT NULL,
    provider_user_id NVARCHAR(128) NOT NULL,
    union_id NVARCHAR(128) NULL,
    credential_hash NVARCHAR(255) NULL,
    bound_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_login_at DATETIME2 NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_auth_identity_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT uk_auth_provider_user UNIQUE (provider, provider_user_id)
);

CREATE TABLE user_profile (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    english_level NVARCHAR(32) NULL,
    target_level NVARCHAR(32) NULL,
    daily_minutes INT NOT NULL DEFAULT 30,
    timezone NVARCHAR(64) NOT NULL DEFAULT 'Asia/Shanghai',
    preference_json NVARCHAR(4000) NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_user_profile_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT uk_user_profile_user UNIQUE (user_id)
);

CREATE TABLE user_device (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    device_uuid NVARCHAR(128) NOT NULL,
    device_name NVARCHAR(128) NULL,
    platform NVARCHAR(32) NOT NULL,
    app_version NVARCHAR(64) NULL,
    last_pulled_version BIGINT NOT NULL DEFAULT 0,
    last_seen_at DATETIME2 NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_user_device_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT uk_user_device_uuid UNIQUE (user_id, device_uuid)
);

CREATE TABLE email_verification (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NULL,
    email NVARCHAR(128) NOT NULL,
    purpose NVARCHAR(32) NOT NULL,
    code_hash NVARCHAR(255) NOT NULL,
    send_status NVARCHAR(32) NOT NULL DEFAULT 'PENDING',
    send_count INT NOT NULL DEFAULT 0,
    expires_at DATETIME2 NOT NULL,
    consumed_at DATETIME2 NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_email_verification_user FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE TABLE notification_message (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NULL,
    channel NVARCHAR(32) NOT NULL,
    template_code NVARCHAR(64) NULL,
    receiver NVARCHAR(128) NOT NULL,
    title NVARCHAR(200) NULL,
    content NVARCHAR(2000) NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'PENDING',
    sent_at DATETIME2 NULL,
    error_message NVARCHAR(500) NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_notification_message_user FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE INDEX idx_users_email ON users(email);
CREATE INDEX idx_users_phone ON users(phone);
CREATE INDEX idx_users_status ON users(status);
CREATE INDEX idx_auth_identity_user ON user_auth_identity(user_id);
CREATE INDEX idx_email_verification_email ON email_verification(email, purpose);
CREATE INDEX idx_user_device_user_seen ON user_device(user_id, last_seen_at);
