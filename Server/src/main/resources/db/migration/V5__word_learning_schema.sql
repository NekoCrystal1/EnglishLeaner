CREATE TABLE user_word_plan (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    book_id BIGINT NOT NULL,
    daily_new_count INT NOT NULL DEFAULT 10,
    daily_review_count INT NOT NULL DEFAULT 20,
    current_position INT NOT NULL DEFAULT 0,
    review_algorithm NVARCHAR(32) NOT NULL DEFAULT 'SM2_SIMPLE',
    started_on DATE NOT NULL,
    finished_on DATE NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_word_plan_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_word_plan_book FOREIGN KEY (book_id) REFERENCES book(id)
);

CREATE TABLE user_word_state (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    plan_id BIGINT NOT NULL,
    book_id BIGINT NOT NULL,
    word_id BIGINT NOT NULL,
    state NVARCHAR(32) NOT NULL DEFAULT 'NEW',
    first_seen_at DATETIME2 NULL,
    last_answered_at DATETIME2 NULL,
    next_review_at DATETIME2 NULL,
    exposure_count INT NOT NULL DEFAULT 0,
    correct_count INT NOT NULL DEFAULT 0,
    wrong_count INT NOT NULL DEFAULT 0,
    mastery_level INT NOT NULL DEFAULT 0,
    ease_factor FLOAT NOT NULL DEFAULT 2.5,
    interval_days INT NOT NULL DEFAULT 0,
    sync_version BIGINT NOT NULL DEFAULT 0,
    source_device_id BIGINT NULL,
    client_updated_at DATETIME2 NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_word_state_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_word_state_plan FOREIGN KEY (plan_id) REFERENCES user_word_plan(id),
    CONSTRAINT fk_word_state_book FOREIGN KEY (book_id) REFERENCES book(id),
    CONSTRAINT fk_word_state_word FOREIGN KEY (word_id) REFERENCES vocabulary_word(id),
    CONSTRAINT uk_user_plan_word UNIQUE (user_id, plan_id, word_id)
);

CREATE TABLE word_study_event (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    plan_id BIGINT NULL,
    book_id BIGINT NULL,
    word_id BIGINT NOT NULL,
    activity_type NVARCHAR(32) NOT NULL,
    question_id BIGINT NULL,
    selected_answer NVARCHAR(256) NULL,
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
    CONSTRAINT fk_word_event_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_word_event_plan FOREIGN KEY (plan_id) REFERENCES user_word_plan(id),
    CONSTRAINT fk_word_event_book FOREIGN KEY (book_id) REFERENCES book(id),
    CONSTRAINT fk_word_event_word FOREIGN KEY (word_id) REFERENCES vocabulary_word(id)
);

CREATE TABLE score_transaction (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    source_type NVARCHAR(32) NOT NULL,
    source_id BIGINT NULL,
    score_delta INT NOT NULL,
    balance_after INT NOT NULL,
    reason NVARCHAR(200) NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_score_transaction_user FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE TABLE daily_study_summary (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    study_date DATE NOT NULL,
    new_words INT NOT NULL DEFAULT 0,
    review_words INT NOT NULL DEFAULT 0,
    correct_count INT NOT NULL DEFAULT 0,
    wrong_count INT NOT NULL DEFAULT 0,
    study_seconds INT NOT NULL DEFAULT 0,
    score_delta INT NOT NULL DEFAULT 0,
    completed_tasks INT NOT NULL DEFAULT 0,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_daily_summary_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT uk_daily_user_date UNIQUE (user_id, study_date)
);

CREATE INDEX idx_word_plan_user_status ON user_word_plan(user_id, status);
CREATE INDEX idx_word_plan_book ON user_word_plan(book_id);
CREATE INDEX idx_word_state_due ON user_word_state(user_id, plan_id, next_review_at);
CREATE INDEX idx_word_state_user_state ON user_word_state(user_id, state);
CREATE INDEX idx_word_event_user_time ON word_study_event(user_id, answered_at);
CREATE INDEX idx_word_event_word ON word_study_event(word_id);
CREATE INDEX idx_score_transaction_user_time ON score_transaction(user_id, created_at);
