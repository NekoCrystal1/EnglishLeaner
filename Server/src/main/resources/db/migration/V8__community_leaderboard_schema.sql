CREATE TABLE study_group (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    owner_user_id BIGINT NOT NULL,
    name NVARCHAR(100) NOT NULL,
    description NVARCHAR(500) NULL,
    join_mode NVARCHAR(32) NOT NULL DEFAULT 'INVITE',
    invite_code NVARCHAR(64) NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_study_group_owner FOREIGN KEY (owner_user_id) REFERENCES users(id)
);

CREATE TABLE study_group_member (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    group_id BIGINT NOT NULL,
    user_id BIGINT NOT NULL,
    member_role NVARCHAR(32) NOT NULL DEFAULT 'MEMBER',
    joined_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_group_member_group FOREIGN KEY (group_id) REFERENCES study_group(id),
    CONSTRAINT fk_group_member_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT uk_group_member UNIQUE (group_id, user_id)
);

CREATE TABLE study_partner (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    partner_user_id BIGINT NOT NULL,
    relation_type NVARCHAR(32) NOT NULL DEFAULT 'PARTNER',
    status NVARCHAR(32) NOT NULL DEFAULT 'PENDING',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_study_partner_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_study_partner_partner FOREIGN KEY (partner_user_id) REFERENCES users(id),
    CONSTRAINT uk_study_partner_pair UNIQUE (user_id, partner_user_id)
);

CREATE TABLE checkin_record (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    checkin_date DATE NOT NULL,
    streak_days INT NOT NULL DEFAULT 1,
    reward_score INT NOT NULL DEFAULT 0,
    source NVARCHAR(32) NOT NULL DEFAULT 'DAILY',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_checkin_record_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT uk_checkin_user_date UNIQUE (user_id, checkin_date)
);

CREATE TABLE achievement (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    code NVARCHAR(64) NOT NULL,
    name NVARCHAR(100) NOT NULL,
    description NVARCHAR(500) NULL,
    condition_json NVARCHAR(4000) NULL,
    reward_score INT NOT NULL DEFAULT 0,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT uk_achievement_code UNIQUE (code)
);

CREATE TABLE user_achievement (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    achievement_id BIGINT NOT NULL,
    achieved_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    progress_json NVARCHAR(4000) NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_user_achievement_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_user_achievement_achievement FOREIGN KEY (achievement_id) REFERENCES achievement(id),
    CONSTRAINT uk_user_achievement UNIQUE (user_id, achievement_id)
);

CREATE TABLE leaderboard_snapshot (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    scope_type NVARCHAR(32) NOT NULL,
    scope_id BIGINT NULL,
    period_type NVARCHAR(32) NOT NULL,
    period_start DATE NULL,
    period_end DATE NULL,
    rank_date DATE NOT NULL,
    user_id BIGINT NOT NULL,
    rank_no INT NOT NULL,
    score INT NOT NULL DEFAULT 0,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_leaderboard_user FOREIGN KEY (user_id) REFERENCES users(id)
);

ALTER TABLE content_share ADD CONSTRAINT fk_content_share_target_group FOREIGN KEY (target_group_id) REFERENCES study_group(id);

CREATE INDEX idx_study_group_owner ON study_group(owner_user_id);
CREATE INDEX idx_study_group_invite_code ON study_group(invite_code);
CREATE INDEX idx_group_member_user ON study_group_member(user_id, status);
CREATE INDEX idx_study_partner_user ON study_partner(user_id, status);
CREATE INDEX idx_checkin_user_date ON checkin_record(user_id, checkin_date);
CREATE INDEX idx_user_achievement_user ON user_achievement(user_id, achieved_at);
CREATE INDEX idx_leaderboard_scope_period ON leaderboard_snapshot(scope_type, scope_id, period_type, period_start, period_end, rank_no);
CREATE INDEX idx_leaderboard_user ON leaderboard_snapshot(user_id, rank_date);
