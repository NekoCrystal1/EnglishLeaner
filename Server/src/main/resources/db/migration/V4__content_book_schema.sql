CREATE TABLE media_asset (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    owner_user_id BIGINT NULL,
    media_type NVARCHAR(32) NOT NULL,
    storage_provider NVARCHAR(32) NOT NULL DEFAULT 'LOCAL',
    url NVARCHAR(512) NULL,
    object_key NVARCHAR(512) NULL,
    mime_type NVARCHAR(128) NULL,
    duration_ms INT NULL,
    size_bytes BIGINT NULL,
    hash NVARCHAR(128) NULL,
    status NVARCHAR(32) NOT NULL DEFAULT 'ACTIVE',
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_media_asset_owner FOREIGN KEY (owner_user_id) REFERENCES users(id)
);

CREATE TABLE book (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    title NVARCHAR(200) NOT NULL,
    book_type NVARCHAR(32) NOT NULL,
    source_type NVARCHAR(32) NOT NULL,
    owner_user_id BIGINT NULL,
    language NVARCHAR(32) NOT NULL DEFAULT 'en',
    level NVARCHAR(32) NULL,
    cover_url NVARCHAR(512) NULL,
    item_count INT NOT NULL DEFAULT 0,
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
    CONSTRAINT fk_book_owner FOREIGN KEY (owner_user_id) REFERENCES users(id)
);

CREATE TABLE vocabulary_word (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    word NVARCHAR(128) NOT NULL,
    normalized_word NVARCHAR(128) NOT NULL,
    phonetic_us NVARCHAR(128) NULL,
    phonetic_uk NVARCHAR(128) NULL,
    translation NVARCHAR(256) NOT NULL,
    part_of_speech NVARCHAR(64) NULL,
    example_sentence NVARCHAR(1000) NULL,
    difficulty INT NOT NULL DEFAULT 1,
    audio_asset_id BIGINT NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_vocabulary_audio FOREIGN KEY (audio_asset_id) REFERENCES media_asset(id)
);

CREATE TABLE book_word (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    book_id BIGINT NOT NULL,
    word_id BIGINT NOT NULL,
    unit_no INT NOT NULL DEFAULT 1,
    sort_order INT NOT NULL DEFAULT 0,
    is_required BIT NOT NULL DEFAULT 1,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_book_word_book FOREIGN KEY (book_id) REFERENCES book(id),
    CONSTRAINT fk_book_word_word FOREIGN KEY (word_id) REFERENCES vocabulary_word(id),
    CONSTRAINT uk_book_word UNIQUE (book_id, word_id)
);

CREATE TABLE book_item (
    id BIGINT IDENTITY(1,1) PRIMARY KEY,
    book_id BIGINT NOT NULL,
    item_type NVARCHAR(32) NOT NULL,
    item_id BIGINT NOT NULL,
    unit_no INT NOT NULL DEFAULT 1,
    sort_order INT NOT NULL DEFAULT 0,
    title NVARCHAR(200) NULL,
    media_asset_id BIGINT NULL,
    created_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME2 NOT NULL DEFAULT CURRENT_TIMESTAMP,
    version BIGINT NOT NULL DEFAULT 0,
    is_deleted BIT NOT NULL DEFAULT 0,
    ext_json NVARCHAR(4000) NULL,
    remark NVARCHAR(500) NULL,
    CONSTRAINT fk_book_item_book FOREIGN KEY (book_id) REFERENCES book(id),
    CONSTRAINT fk_book_item_media FOREIGN KEY (media_asset_id) REFERENCES media_asset(id)
);

INSERT INTO book (title, book_type, source_type, language, level, item_count, content_storage_mode, status)
VALUES ('基础词汇', 'WORD', 'SYSTEM', 'en', 'BEGINNER', 0, 'SERVER', 'ACTIVE');

INSERT INTO vocabulary_word (word, normalized_word, translation, difficulty)
SELECT word, LOWER(word), translation, 1
FROM vocabulary
WHERE is_deleted = 0;

INSERT INTO book_word (book_id, word_id, unit_no, sort_order, is_required)
SELECT b.id, vw.id, 1, ROW_NUMBER() OVER (ORDER BY vw.id), 1
FROM book b
CROSS JOIN vocabulary_word vw
WHERE b.title = '基础词汇' AND b.source_type = 'SYSTEM';

UPDATE book
SET item_count = (SELECT COUNT(*) FROM book_word WHERE book_word.book_id = book.id AND book_word.is_deleted = 0)
WHERE title = '基础词汇' AND source_type = 'SYSTEM';

CREATE INDEX idx_book_type_status ON book(book_type, status);
CREATE INDEX idx_book_owner ON book(owner_user_id);
CREATE INDEX idx_word_normalized ON vocabulary_word(normalized_word);
CREATE INDEX idx_word_difficulty ON vocabulary_word(difficulty);
CREATE INDEX idx_book_word_order ON book_word(book_id, sort_order);
CREATE INDEX idx_book_item_order ON book_item(book_id, sort_order);
