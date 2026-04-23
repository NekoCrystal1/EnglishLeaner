CREATE TABLE users (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    username NVARCHAR(64) NOT NULL,
    password_hash NVARCHAR(255) NOT NULL,
    total_score INT NOT NULL DEFAULT 0,
    study_days INT NOT NULL DEFAULT 0,
    role_name NVARCHAR(32) NOT NULL DEFAULT 'USER',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT uk_users_username UNIQUE (username)
);

CREATE TABLE vocabulary (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    word NVARCHAR(128) NOT NULL,
    translation NVARCHAR(256) NOT NULL,
    option1 NVARCHAR(256) NULL,
    option2 NVARCHAR(256) NULL,
    option3 NVARCHAR(256) NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL
);

CREATE TABLE study_log (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    word_id BIGINT NOT NULL,
    selected_answer NVARCHAR(256) NULL,
    is_correct BIT NOT NULL,
    score_delta INT NOT NULL DEFAULT 0,
    answered_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_study_log_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_study_log_word FOREIGN KEY (word_id) REFERENCES vocabulary(id)
);

CREATE TABLE sync_task_log (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    device_id NVARCHAR(128) NOT NULL,
    task_type NVARCHAR(32) NOT NULL,
    status NVARCHAR(32) NOT NULL,
    payload_json NVARCHAR(4000) NULL,
    error_message NVARCHAR(500) NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL
);

CREATE INDEX idx_users_score ON users(total_score);
CREATE INDEX idx_study_log_user_time ON study_log(user_id, answered_at);
CREATE INDEX idx_study_log_word ON study_log(word_id);
