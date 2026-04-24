CREATE TABLE content_share (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    owner_user_id BIGINT NOT NULL,
    target_user_id BIGINT NULL,
    target_group_id BIGINT NULL,
    target_type NVARCHAR(32) NOT NULL,
    target_id BIGINT NOT NULL,
    share_token NVARCHAR(128) NOT NULL,
    transfer_mode NVARCHAR(32) NOT NULL DEFAULT 'CLIENT_DIRECT',
    expires_at DATETIME2 NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_content_share_owner FOREIGN KEY (owner_user_id) REFERENCES users(id),
    CONSTRAINT fk_content_share_target_user FOREIGN KEY (target_user_id) REFERENCES users(id),
    CONSTRAINT uk_content_share_token UNIQUE (share_token)
);

CREATE TABLE reading_passage (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    book_id BIGINT NULL,
    owner_user_id BIGINT NULL,
    source_type NVARCHAR(32) NOT NULL DEFAULT 'SYSTEM',
    title NVARCHAR(200) NOT NULL,
    content NVARCHAR(MAX) NULL,
    summary NVARCHAR(1000) NULL,
    language NVARCHAR(32) NOT NULL DEFAULT 'en',
    level NVARCHAR(32) NULL,
    word_count INT NOT NULL DEFAULT 0,
    estimated_minutes INT NULL,
    content_storage_mode NVARCHAR(32) NOT NULL DEFAULT 'SERVER',
    client_content_ref NVARCHAR(512) NULL,
    content_hash NVARCHAR(128) NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_reading_passage_book FOREIGN KEY (book_id) REFERENCES book(id),
    CONSTRAINT fk_reading_passage_owner FOREIGN KEY (owner_user_id) REFERENCES users(id)
);

CREATE TABLE reading_progress (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    passage_id BIGINT NOT NULL,
    progress_percent INT NOT NULL DEFAULT 0,
    last_position NVARCHAR(128) NULL,
    read_seconds INT NOT NULL DEFAULT 0,
    completed_at DATETIME2 NULL,
    last_read_at DATETIME2 NULL,
    client_event_id NVARCHAR(128) NULL,
    source_device_id BIGINT NULL,
    client_updated_at DATETIME2 NULL,
    sync_version BIGINT NOT NULL DEFAULT 0,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_reading_progress_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_reading_progress_passage FOREIGN KEY (passage_id) REFERENCES reading_passage(id),
    CONSTRAINT uk_reading_progress_user_passage UNIQUE (user_id, passage_id)
);

CREATE TABLE listening_material (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    book_id BIGINT NULL,
    owner_user_id BIGINT NULL,
    source_type NVARCHAR(32) NOT NULL DEFAULT 'SYSTEM',
    title NVARCHAR(200) NOT NULL,
    transcript NVARCHAR(MAX) NULL,
    media_asset_id BIGINT NULL,
    level NVARCHAR(32) NULL,
    duration_ms INT NULL,
    content_storage_mode NVARCHAR(32) NOT NULL DEFAULT 'SERVER',
    client_content_ref NVARCHAR(512) NULL,
    content_hash NVARCHAR(128) NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_listening_material_book FOREIGN KEY (book_id) REFERENCES book(id),
    CONSTRAINT fk_listening_material_owner FOREIGN KEY (owner_user_id) REFERENCES users(id),
    CONSTRAINT fk_listening_material_media FOREIGN KEY (media_asset_id) REFERENCES media_asset(id)
);

CREATE TABLE listening_progress (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    material_id BIGINT NOT NULL,
    listen_seconds INT NOT NULL DEFAULT 0,
    completed_count INT NOT NULL DEFAULT 0,
    last_position_ms INT NOT NULL DEFAULT 0,
    last_listened_at DATETIME2 NULL,
    client_event_id NVARCHAR(128) NULL,
    source_device_id BIGINT NULL,
    client_updated_at DATETIME2 NULL,
    sync_version BIGINT NOT NULL DEFAULT 0,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_listening_progress_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_listening_progress_material FOREIGN KEY (material_id) REFERENCES listening_material(id),
    CONSTRAINT uk_listening_progress_user_material UNIQUE (user_id, material_id)
);

CREATE TABLE speaking_task (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    owner_user_id BIGINT NULL,
    source_type NVARCHAR(32) NOT NULL DEFAULT 'SYSTEM',
    title NVARCHAR(200) NOT NULL,
    prompt_text NVARCHAR(2000) NULL,
    reference_text NVARCHAR(2000) NULL,
    media_asset_id BIGINT NULL,
    level NVARCHAR(32) NULL,
    score_config_json NVARCHAR(4000) NULL,
    content_storage_mode NVARCHAR(32) NOT NULL DEFAULT 'SERVER',
    client_content_ref NVARCHAR(512) NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_speaking_task_owner FOREIGN KEY (owner_user_id) REFERENCES users(id),
    CONSTRAINT fk_speaking_task_media FOREIGN KEY (media_asset_id) REFERENCES media_asset(id)
);

CREATE TABLE speaking_attempt (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    task_id BIGINT NOT NULL,
    audio_asset_id BIGINT NULL,
    evaluation_mode NVARCHAR(32) NOT NULL DEFAULT 'LOCAL',
    evaluation_provider NVARCHAR(64) NULL,
    recognized_text NVARCHAR(2000) NULL,
    score_total INT NULL,
    score_detail_json NVARCHAR(4000) NULL,
    duration_ms INT NULL,
    attempted_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    client_event_id NVARCHAR(128) NULL,
    source_device_id BIGINT NULL,
    client_updated_at DATETIME2 NULL,
    sync_version BIGINT NOT NULL DEFAULT 0,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_speaking_attempt_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_speaking_attempt_task FOREIGN KEY (task_id) REFERENCES speaking_task(id),
    CONSTRAINT fk_speaking_attempt_audio FOREIGN KEY (audio_asset_id) REFERENCES media_asset(id)
);

CREATE INDEX idx_content_share_owner ON content_share(owner_user_id);
CREATE INDEX idx_content_share_target ON content_share(target_type, target_id);
CREATE INDEX idx_content_share_user ON content_share(target_user_id, status);
CREATE INDEX idx_reading_passage_source ON reading_passage(source_type, owner_user_id, status);
CREATE INDEX idx_reading_progress_user ON reading_progress(user_id, last_read_at);
CREATE INDEX idx_listening_material_source ON listening_material(source_type, owner_user_id, status);
CREATE INDEX idx_listening_progress_user ON listening_progress(user_id, last_listened_at);
CREATE INDEX idx_speaking_task_source ON speaking_task(source_type, owner_user_id, status);
CREATE INDEX idx_speaking_attempt_user_time ON speaking_attempt(user_id, attempted_at);
