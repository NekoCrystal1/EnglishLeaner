CREATE TABLE question (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    module_type NVARCHAR(32) NOT NULL,
    question_type NVARCHAR(32) NOT NULL,
    source_type NVARCHAR(32) NULL,
    source_id BIGINT NULL,
    stem NVARCHAR(2000) NOT NULL,
    answer_text NVARCHAR(1000) NULL,
    explanation NVARCHAR(2000) NULL,
    difficulty INT NOT NULL DEFAULT 1,
    media_asset_id BIGINT NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_question_media FOREIGN KEY (media_asset_id) REFERENCES media_asset(id)
);

CREATE TABLE question_option (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    question_id BIGINT NOT NULL,
    option_key NVARCHAR(16) NOT NULL,
    option_text NVARCHAR(1000) NOT NULL,
    is_correct BIT NOT NULL DEFAULT 0,
    sort_order INT NOT NULL DEFAULT 0,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_question_option_question FOREIGN KEY (question_id) REFERENCES question(id),
    CONSTRAINT uk_question_option_key UNIQUE (question_id, option_key)
);

CREATE TABLE question_attempt (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    question_id BIGINT NOT NULL,
    module_type NVARCHAR(32) NOT NULL,
    selected_option_id BIGINT NULL,
    answer_payload_json NVARCHAR(4000) NULL,
    is_correct BIT NOT NULL,
    score_delta INT NOT NULL DEFAULT 0,
    duration_ms INT NULL,
    answered_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
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
    CONSTRAINT fk_question_attempt_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_question_attempt_question FOREIGN KEY (question_id) REFERENCES question(id),
    CONSTRAINT fk_question_attempt_option FOREIGN KEY (selected_option_id) REFERENCES question_option(id)
);

CREATE TABLE wrong_answer (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    question_id BIGINT NOT NULL,
    module_type NVARCHAR(32) NOT NULL,
    first_wrong_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_wrong_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    wrong_count INT NOT NULL DEFAULT 1,
    resolved_at DATETIME2 NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_wrong_answer_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_wrong_answer_question FOREIGN KEY (question_id) REFERENCES question(id),
    CONSTRAINT uk_wrong_answer_user_question UNIQUE (user_id, question_id)
);

CREATE TABLE favorite_item (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    target_type NVARCHAR(32) NOT NULL,
    target_id BIGINT NOT NULL,
    folder_name NVARCHAR(100) NULL,
    favorited_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_favorite_item_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT uk_favorite_item UNIQUE (user_id, target_type, target_id)
);

CREATE INDEX idx_question_module_status ON question(module_type, status);
CREATE INDEX idx_question_source ON question(source_type, source_id);
CREATE INDEX idx_question_attempt_user_time ON question_attempt(user_id, answered_at);
CREATE INDEX idx_question_attempt_question ON question_attempt(question_id);
CREATE INDEX idx_question_attempt_client ON question_attempt(user_id, client_event_id);
CREATE INDEX idx_wrong_answer_user_status ON wrong_answer(user_id, status);
CREATE INDEX idx_favorite_user_target ON favorite_item(user_id, target_type, target_id);
