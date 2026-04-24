#include "LocalStore.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QUuid>
#include <QVariant>

LocalStore::LocalStore()
    : m_connectionName(QStringLiteral("english_learner_local_store_%1")
                           .arg(reinterpret_cast<quintptr>(this)))
{
}

LocalStore::~LocalStore()
{
    if (m_db.isValid()) {
        m_db.close();
    }
}

bool LocalStore::openForUser(const UserProfile& user, QString* errorMessage)
{
    m_userId = user.userId > 0 ? user.userId : 1;

    const QString dataRoot = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataRoot.isEmpty()
                 ? QDir(QCoreApplication::applicationDirPath()).filePath("local-data")
                 : dataRoot);
    if (!dir.exists() && !dir.mkpath(".")) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("无法创建本地数据目录：%1").arg(dir.absolutePath());
        }
        return false;
    }

    m_databasePath = dir.filePath(QStringLiteral("english_learner_%1.sqlite").arg(m_userId));

    if (QSqlDatabase::contains(m_connectionName)) {
        m_db = QSqlDatabase::database(m_connectionName);
    } else {
        m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    }
    m_db.setDatabaseName(m_databasePath);

    if (!m_db.open()) {
        if (errorMessage != nullptr) {
            *errorMessage = m_db.lastError().text();
        }
        return false;
    }

    if (!initializeSchema(errorMessage)) {
        return false;
    }
    if (!seedDefaults(errorMessage)) {
        return false;
    }
    return saveUserProfile(user);
}

bool LocalStore::isOpen() const
{
    return m_db.isValid() && m_db.isOpen();
}

QString LocalStore::databasePath() const
{
    return m_databasePath;
}

QString LocalStore::deviceUuid()
{
    if (!isOpen()) {
        return QString();
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT meta_value FROM local_sync_meta WHERE meta_key = 'device_uuid'"));
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }

    const QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QSqlQuery insert(m_db);
    insert.prepare(QStringLiteral("INSERT OR REPLACE INTO local_sync_meta(meta_key, meta_value) VALUES('device_uuid', ?)"));
    insert.addBindValue(uuid);
    insert.exec();
    return uuid;
}

bool LocalStore::saveUserProfile(const UserProfile& user)
{
    if (!isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
INSERT OR REPLACE INTO local_user_profile(
    user_id, username, display_name, email, total_score, study_days, updated_at
) VALUES (?, ?, ?, ?, ?, ?, ?)
)"));
    query.addBindValue(user.userId);
    query.addBindValue(user.username);
    query.addBindValue(user.displayName);
    query.addBindValue(user.email);
    query.addBindValue(user.totalScore);
    query.addBindValue(user.studyDays);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    return query.exec();
}

QList<LocalBook> LocalStore::books() const
{
    QList<LocalBook> result;
    if (!isOpen()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.exec(QStringLiteral(R"(
SELECT local_id, title, level, source_type, content_storage_mode, item_count,
       learned_count, due_count, mastered_count
FROM local_book
ORDER BY local_id
)"));
    while (query.next()) {
        LocalBook book;
        book.localId = query.value(0).toLongLong();
        book.title = query.value(1).toString();
        book.level = query.value(2).toString();
        book.sourceType = query.value(3).toString();
        book.storageMode = query.value(4).toString();
        book.itemCount = query.value(5).toInt();
        book.learnedCount = query.value(6).toInt();
        book.dueCount = query.value(7).toInt();
        book.masteredCount = query.value(8).toInt();
        result.append(book);
    }
    return result;
}

QList<LocalReadingItem> LocalStore::readingItems() const
{
    QList<LocalReadingItem> result;
    if (!isOpen()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.exec(QStringLiteral(R"(
SELECT local_id, title, level, source_type, word_count, estimated_minutes,
       progress_percent, content
FROM local_reading_item
ORDER BY local_id
)"));
    while (query.next()) {
        LocalReadingItem item;
        item.localId = query.value(0).toLongLong();
        item.title = query.value(1).toString();
        item.level = query.value(2).toString();
        item.sourceType = query.value(3).toString();
        item.wordCount = query.value(4).toInt();
        item.estimatedMinutes = query.value(5).toInt();
        item.progressPercent = query.value(6).toInt();
        item.content = query.value(7).toString();
        result.append(item);
    }
    return result;
}

QList<LocalListeningItem> LocalStore::listeningItems() const
{
    QList<LocalListeningItem> result;
    if (!isOpen()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.exec(QStringLiteral(R"(
SELECT local_id, title, level, duration_seconds, listen_seconds,
       completed_count, transcript
FROM local_listening_item
ORDER BY local_id
)"));
    while (query.next()) {
        LocalListeningItem item;
        item.localId = query.value(0).toLongLong();
        item.title = query.value(1).toString();
        item.level = query.value(2).toString();
        item.durationSeconds = query.value(3).toInt();
        item.listenSeconds = query.value(4).toInt();
        item.completedCount = query.value(5).toInt();
        item.transcript = query.value(6).toString();
        result.append(item);
    }
    return result;
}

QList<LocalSpeakingTask> LocalStore::speakingTasks() const
{
    QList<LocalSpeakingTask> result;
    if (!isOpen()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.exec(QStringLiteral(R"(
SELECT local_id, title, level, prompt, reference_text, last_mode, last_score
FROM local_speaking_task
ORDER BY local_id
)"));
    while (query.next()) {
        LocalSpeakingTask task;
        task.localId = query.value(0).toLongLong();
        task.title = query.value(1).toString();
        task.level = query.value(2).toString();
        task.prompt = query.value(3).toString();
        task.referenceText = query.value(4).toString();
        task.lastMode = query.value(5).toString();
        task.lastScore = query.value(6).toInt();
        result.append(task);
    }
    return result;
}

QList<LocalCommunityItem> LocalStore::communityItems() const
{
    QList<LocalCommunityItem> result;
    if (!isOpen()) {
        return result;
    }

    QSqlQuery query(m_db);
    query.exec(QStringLiteral(R"(
SELECT local_id, name, kind, status
FROM local_community_item
ORDER BY local_id
)"));
    while (query.next()) {
        LocalCommunityItem item;
        item.localId = query.value(0).toLongLong();
        item.name = query.value(1).toString();
        item.kind = query.value(2).toString();
        item.status = query.value(3).toString();
        result.append(item);
    }
    return result;
}

LocalStudySummary LocalStore::todaySummary() const
{
    LocalStudySummary summary;
    if (!isOpen()) {
        return summary;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
SELECT new_words, review_words, correct_count, wrong_count, study_seconds,
       score_delta, completed_tasks
FROM local_daily_summary
WHERE study_date = ?
)"));
    query.addBindValue(todayIso());
    if (query.exec() && query.next()) {
        summary.newWords = query.value(0).toInt();
        summary.reviewWords = query.value(1).toInt();
        summary.correctCount = query.value(2).toInt();
        summary.wrongCount = query.value(3).toInt();
        summary.studySeconds = query.value(4).toInt();
        summary.scoreDelta = query.value(5).toInt();
        summary.completedTasks = query.value(6).toInt();
    }
    return summary;
}

bool LocalStore::savePlan(qint64 bookId, int dailyNewCount, int dailyReviewCount)
{
    if (!isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
INSERT OR REPLACE INTO local_user_word_plan(
    local_id, user_id, book_id, daily_new_count, daily_review_count,
    current_position, review_algorithm, status, updated_at
) VALUES (1, ?, ?, ?, ?, 0, 'SM2_SIMPLE', 'ACTIVE', ?)
)"));
    query.addBindValue(m_userId);
    query.addBindValue(bookId);
    query.addBindValue(dailyNewCount);
    query.addBindValue(dailyReviewCount);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!query.exec()) {
        return false;
    }

    const QJsonObject payload{
        {QStringLiteral("bookId"), QString::number(bookId)},
        {QStringLiteral("dailyNewCount"), dailyNewCount},
        {QStringLiteral("dailyReviewCount"), dailyReviewCount}
    };
    return enqueueOutbox(QStringLiteral("USER_WORD_PLAN"), QStringLiteral("UPSERT"),
                         QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact)));
}

bool LocalStore::recordWordStudy(qint64 userId,
                                 const QuizQuestion& question,
                                 const QString& selectedAnswer,
                                 bool correct,
                                 int scoreDelta,
                                 const QString& activityType)
{
    if (!isOpen()) {
        return false;
    }

    const QString eventId = createClientEventId();
    const QString now = QDateTime::currentDateTime().toString(Qt::ISODate);

    QSqlQuery eventQuery(m_db);
    eventQuery.prepare(QStringLiteral(R"(
INSERT INTO local_word_study_event(
    client_event_id, user_id, word_id, word, activity_type, selected_answer,
    is_correct, score_delta, answered_at, sync_status
) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, 'PENDING')
)"));
    eventQuery.addBindValue(eventId);
    eventQuery.addBindValue(userId);
    eventQuery.addBindValue(question.wordId);
    eventQuery.addBindValue(question.word);
    eventQuery.addBindValue(activityType);
    eventQuery.addBindValue(selectedAnswer);
    eventQuery.addBindValue(correct ? 1 : 0);
    eventQuery.addBindValue(scoreDelta);
    eventQuery.addBindValue(now);
    if (!eventQuery.exec()) {
        return false;
    }

    QSqlQuery attemptQuery(m_db);
    attemptQuery.prepare(QStringLiteral(R"(
INSERT INTO local_question_attempt(
    client_event_id, user_id, question_key, module_type, selected_answer,
    is_correct, score_delta, answered_at, sync_status
) VALUES (?, ?, ?, 'WORD', ?, ?, ?, ?, 'PENDING')
)"));
    attemptQuery.addBindValue(eventId);
    attemptQuery.addBindValue(userId);
    attemptQuery.addBindValue(QString::number(question.wordId));
    attemptQuery.addBindValue(selectedAnswer);
    attemptQuery.addBindValue(correct ? 1 : 0);
    attemptQuery.addBindValue(scoreDelta);
    attemptQuery.addBindValue(now);
    attemptQuery.exec();

    QSqlQuery updateBook(m_db);
    updateBook.prepare(QStringLiteral(R"(
UPDATE local_book
SET learned_count = MIN(item_count, learned_count + 1),
    due_count = MAX(0, due_count - CASE WHEN ? = 'REVIEW' THEN 1 ELSE 0 END),
    mastered_count = MIN(item_count, mastered_count + CASE WHEN ? THEN 1 ELSE 0 END)
WHERE local_id = (SELECT book_id FROM local_user_word_plan WHERE local_id = 1)
)"));
    updateBook.addBindValue(activityType);
    updateBook.addBindValue(correct ? 1 : 0);
    updateBook.exec();

    incrementTodaySummary(activityType == QStringLiteral("REVIEW") ? 0 : 1,
                          activityType == QStringLiteral("REVIEW") ? 1 : 0,
                          correct,
                          45,
                          scoreDelta,
                          1);

    const QJsonObject payload{
        {QStringLiteral("clientEventId"), eventId},
        {QStringLiteral("wordId"), QString::number(question.wordId)},
        {QStringLiteral("word"), question.word},
        {QStringLiteral("selectedAnswer"), selectedAnswer},
        {QStringLiteral("isCorrect"), correct},
        {QStringLiteral("scoreDelta"), scoreDelta}
    };
    return enqueueOutbox(QStringLiteral("WORD_STUDY_EVENT"), QStringLiteral("INSERT"),
                         QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact)));
}

bool LocalStore::completeReading(qint64 readingId)
{
    if (!isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
UPDATE local_reading_item
SET progress_percent = 100, completed_at = ?
WHERE local_id = ?
)"));
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(readingId);
    if (!query.exec()) {
        return false;
    }

    incrementTodaySummary(0, 0, true, 360, 3, 1);
    return enqueueOutbox(QStringLiteral("READING_PROGRESS"), QStringLiteral("UPSERT"),
                         QStringLiteral("{\"readingId\":%1,\"progress\":100}").arg(readingId));
}

bool LocalStore::completeListening(qint64 listeningId)
{
    if (!isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
UPDATE local_listening_item
SET listen_seconds = duration_seconds, completed_count = completed_count + 1,
    last_listened_at = ?
WHERE local_id = ?
)"));
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(listeningId);
    if (!query.exec()) {
        return false;
    }

    incrementTodaySummary(0, 0, true, 240, 2, 1);
    return enqueueOutbox(QStringLiteral("LISTENING_PROGRESS"), QStringLiteral("UPSERT"),
                         QStringLiteral("{\"materialId\":%1,\"completed\":true}").arg(listeningId));
}

bool LocalStore::saveSpeakingAttempt(qint64 taskId, int score, const QString& mode)
{
    if (!isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
UPDATE local_speaking_task
SET last_score = ?, last_mode = ?, attempted_at = ?
WHERE local_id = ?
)"));
    query.addBindValue(score);
    query.addBindValue(mode);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(taskId);
    if (!query.exec()) {
        return false;
    }

    incrementTodaySummary(0, 0, score >= 70, 180, score >= 70 ? 3 : 1, 1);
    return enqueueOutbox(QStringLiteral("SPEAKING_ATTEMPT"), QStringLiteral("INSERT"),
                         QStringLiteral("{\"taskId\":%1,\"evaluationMode\":\"%2\",\"score\":%3}")
                             .arg(taskId)
                             .arg(mode)
                             .arg(score));
}

bool LocalStore::checkIn(qint64 userId, int rewardScore)
{
    if (!isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
INSERT OR IGNORE INTO local_checkin_record(
    user_id, checkin_date, streak_days, reward_score, source
) VALUES (?, ?, 1, ?, 'QT_CLIENT')
)"));
    query.addBindValue(userId);
    query.addBindValue(todayIso());
    query.addBindValue(rewardScore);
    if (!query.exec()) {
        return false;
    }

    incrementTodaySummary(0, 0, true, 0, rewardScore, 0);
    return enqueueOutbox(QStringLiteral("CHECKIN_RECORD"), QStringLiteral("INSERT"),
                         QStringLiteral("{\"date\":\"%1\",\"rewardScore\":%2}")
                             .arg(todayIso())
                             .arg(rewardScore));
}

bool LocalStore::createShareRecord(const QString& targetType, const QString& targetName)
{
    if (!isOpen()) {
        return false;
    }

    const QString token = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
INSERT INTO local_content_share(
    owner_user_id, target_type, target_name, share_token, transfer_mode,
    status, created_at
) VALUES (?, ?, ?, ?, 'CLIENT_DIRECT', 'READY', ?)
)"));
    query.addBindValue(m_userId);
    query.addBindValue(targetType);
    query.addBindValue(targetName);
    query.addBindValue(token);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!query.exec()) {
        return false;
    }

    return enqueueOutbox(QStringLiteral("CONTENT_SHARE"), QStringLiteral("CREATE"),
                         QStringLiteral("{\"targetType\":\"%1\",\"targetName\":\"%2\",\"shareToken\":\"%3\"}")
                             .arg(targetType, targetName, token));
}

bool LocalStore::createStudyGroup(const QString& name)
{
    if (!isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
INSERT INTO local_community_item(name, kind, status)
VALUES (?, 'CUSTOM_GROUP', 'ACTIVE')
)"));
    query.addBindValue(name);
    if (!query.exec()) {
        return false;
    }
    return enqueueOutbox(QStringLiteral("STUDY_GROUP"), QStringLiteral("CREATE"),
                         QStringLiteral("{\"name\":\"%1\"}").arg(name));
}

bool LocalStore::markAllSynced()
{
    if (!isOpen()) {
        return false;
    }

    if (!execute(QStringLiteral("UPDATE local_sync_outbox SET status = 'SYNCED', synced_at = datetime('now') WHERE status = 'PENDING'"))) {
        return false;
    }
    execute(QStringLiteral("UPDATE local_word_study_event SET sync_status = 'SYNCED' WHERE sync_status = 'PENDING'"));
    execute(QStringLiteral("UPDATE local_question_attempt SET sync_status = 'SYNCED' WHERE sync_status = 'PENDING'"));
    execute(QStringLiteral("INSERT OR REPLACE INTO local_sync_meta(meta_key, meta_value) VALUES('last_sync_at', datetime('now'))"));
    return true;
}

int LocalStore::pendingOutboxCount() const
{
    if (!isOpen()) {
        return 0;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT COUNT(*) FROM local_sync_outbox WHERE status = 'PENDING'"));
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int LocalStore::unsyncedEventCount() const
{
    if (!isOpen()) {
        return 0;
    }

    QSqlQuery query(m_db);
    query.exec(QStringLiteral(R"(
SELECT
    (SELECT COUNT(*) FROM local_word_study_event WHERE sync_status = 'PENDING') +
    (SELECT COUNT(*) FROM local_question_attempt WHERE sync_status = 'PENDING')
)"));
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool LocalStore::initializeSchema(QString* errorMessage)
{
    const QStringList statements{
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_user_profile (
    user_id INTEGER PRIMARY KEY,
    username TEXT NOT NULL,
    display_name TEXT,
    email TEXT,
    total_score INTEGER NOT NULL DEFAULT 0,
    study_days INTEGER NOT NULL DEFAULT 0,
    updated_at TEXT
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_sync_meta (
    meta_key TEXT PRIMARY KEY,
    meta_value TEXT
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_book (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    server_id INTEGER,
    title TEXT NOT NULL,
    level TEXT,
    source_type TEXT NOT NULL,
    content_storage_mode TEXT NOT NULL,
    content_hash TEXT,
    item_count INTEGER NOT NULL DEFAULT 0,
    learned_count INTEGER NOT NULL DEFAULT 0,
    due_count INTEGER NOT NULL DEFAULT 0,
    mastered_count INTEGER NOT NULL DEFAULT 0
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_user_word_plan (
    local_id INTEGER PRIMARY KEY,
    user_id INTEGER NOT NULL,
    book_id INTEGER NOT NULL,
    daily_new_count INTEGER NOT NULL,
    daily_review_count INTEGER NOT NULL,
    current_position INTEGER NOT NULL DEFAULT 0,
    review_algorithm TEXT,
    status TEXT NOT NULL,
    updated_at TEXT
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_word_study_event (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    client_event_id TEXT NOT NULL UNIQUE,
    user_id INTEGER NOT NULL,
    word_id INTEGER,
    word TEXT,
    activity_type TEXT,
    selected_answer TEXT,
    is_correct INTEGER,
    score_delta INTEGER,
    answered_at TEXT,
    sync_status TEXT NOT NULL
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_question_attempt (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    client_event_id TEXT NOT NULL UNIQUE,
    user_id INTEGER NOT NULL,
    question_key TEXT,
    module_type TEXT,
    selected_answer TEXT,
    is_correct INTEGER,
    score_delta INTEGER,
    answered_at TEXT,
    sync_status TEXT NOT NULL
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_daily_summary (
    study_date TEXT PRIMARY KEY,
    new_words INTEGER NOT NULL DEFAULT 0,
    review_words INTEGER NOT NULL DEFAULT 0,
    correct_count INTEGER NOT NULL DEFAULT 0,
    wrong_count INTEGER NOT NULL DEFAULT 0,
    study_seconds INTEGER NOT NULL DEFAULT 0,
    score_delta INTEGER NOT NULL DEFAULT 0,
    completed_tasks INTEGER NOT NULL DEFAULT 0
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_reading_item (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    level TEXT,
    source_type TEXT,
    word_count INTEGER,
    estimated_minutes INTEGER,
    progress_percent INTEGER NOT NULL DEFAULT 0,
    content TEXT,
    completed_at TEXT
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_listening_item (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    level TEXT,
    duration_seconds INTEGER,
    listen_seconds INTEGER NOT NULL DEFAULT 0,
    completed_count INTEGER NOT NULL DEFAULT 0,
    transcript TEXT,
    last_listened_at TEXT
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_speaking_task (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    level TEXT,
    prompt TEXT,
    reference_text TEXT,
    last_mode TEXT,
    last_score INTEGER NOT NULL DEFAULT 0,
    attempted_at TEXT
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_content_share (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    owner_user_id INTEGER,
    target_type TEXT,
    target_name TEXT,
    share_token TEXT,
    transfer_mode TEXT,
    status TEXT,
    created_at TEXT
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_community_item (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    kind TEXT NOT NULL,
    status TEXT NOT NULL
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_checkin_record (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    checkin_date TEXT NOT NULL,
    streak_days INTEGER,
    reward_score INTEGER,
    source TEXT,
    UNIQUE(user_id, checkin_date)
)
)"),
        QStringLiteral(R"(
CREATE TABLE IF NOT EXISTS local_sync_outbox (
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    entity_type TEXT NOT NULL,
    operation TEXT NOT NULL,
    payload_json TEXT,
    status TEXT NOT NULL,
    created_at TEXT,
    synced_at TEXT
)
)")
    };

    for (const QString& statement : statements) {
        if (!execute(statement, errorMessage)) {
            return false;
        }
    }
    return true;
}

bool LocalStore::seedDefaults(QString* errorMessage)
{
    if (tableIsEmpty(QStringLiteral("local_book"))) {
        const QVariantList rows[] = {
            {QStringLiteral("核心词汇 1200"), QStringLiteral("A2-B1"), QStringLiteral("SYSTEM"), QStringLiteral("SERVER"), QStringLiteral("seed-core"), 1200, 86, 18, 24},
            {QStringLiteral("用户上传：晨读短文词表"), QStringLiteral("B1"), QStringLiteral("USER_UPLOAD"), QStringLiteral("CLIENT_ONLY"), QStringLiteral("local-upload-demo"), 180, 12, 6, 5}
        };
        for (const QVariantList& row : rows) {
            QSqlQuery query(m_db);
            query.prepare(QStringLiteral(R"(
INSERT INTO local_book(title, level, source_type, content_storage_mode, content_hash,
                       item_count, learned_count, due_count, mastered_count)
VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
)"));
            query.addBindValue(row[0]);
            query.addBindValue(row[1]);
            query.addBindValue(row[2]);
            query.addBindValue(row[3]);
            query.addBindValue(row[4]);
            query.addBindValue(row[5]);
            query.addBindValue(row[6]);
            query.addBindValue(row[7]);
            query.addBindValue(row[8]);
            if (!query.exec()) {
                if (errorMessage != nullptr) {
                    *errorMessage = query.lastError().text();
                }
                return false;
            }
        }
    }

    if (tableIsEmpty(QStringLiteral("local_reading_item"))) {
        const QVariantList rows[] = {
            {QStringLiteral("A Rainy Walk"), QStringLiteral("A2"), QStringLiteral("SYSTEM"), 420, 5, 35,
             QStringLiteral("A rainy walk can slow the day down. The city becomes quiet, and every sign looks a little brighter. Read the passage, then answer the comprehension questions.")},
            {QStringLiteral("My Uploaded Article"), QStringLiteral("B1"), QStringLiteral("USER_UPLOAD"), 760, 8, 0,
             QStringLiteral("This item represents a local reading material imported by the learner. The backend keeps metadata and a content hash, while the original text remains on this device.")}
        };
        for (const QVariantList& row : rows) {
            QSqlQuery query(m_db);
            query.prepare(QStringLiteral(R"(
INSERT INTO local_reading_item(title, level, source_type, word_count,
                               estimated_minutes, progress_percent, content)
VALUES (?, ?, ?, ?, ?, ?, ?)
)"));
            for (const QVariant& value : row) {
                query.addBindValue(value);
            }
            if (!query.exec()) {
                if (errorMessage != nullptr) {
                    *errorMessage = query.lastError().text();
                }
                return false;
            }
        }
    }

    if (tableIsEmpty(QStringLiteral("local_listening_item"))) {
        const QVariantList rows[] = {
            {QStringLiteral("Morning Routine"), QStringLiteral("A2"), 180, 60, 0,
             QStringLiteral("I wake up at seven. I drink water, open the window, and read English for ten minutes.")},
            {QStringLiteral("Local Audio: Travel Plan"), QStringLiteral("B1"), 260, 0, 0,
             QStringLiteral("This is a local listening material. Audio files can stay on the client and be shared directly with friends later.")}
        };
        for (const QVariantList& row : rows) {
            QSqlQuery query(m_db);
            query.prepare(QStringLiteral(R"(
INSERT INTO local_listening_item(title, level, duration_seconds,
                                 listen_seconds, completed_count, transcript)
VALUES (?, ?, ?, ?, ?, ?)
)"));
            for (const QVariant& value : row) {
                query.addBindValue(value);
            }
            if (!query.exec()) {
                if (errorMessage != nullptr) {
                    *errorMessage = query.lastError().text();
                }
                return false;
            }
        }
    }

    if (tableIsEmpty(QStringLiteral("local_speaking_task"))) {
        const QVariantList rows[] = {
            {QStringLiteral("Self Introduction"), QStringLiteral("A2"),
             QStringLiteral("Introduce yourself in 30 seconds."),
             QStringLiteral("Hello, my name is... I like learning English because...")},
            {QStringLiteral("Describe Your Study Partner"), QStringLiteral("B1"),
             QStringLiteral("Talk about a friend or study partner who helps you improve."),
             QStringLiteral("My study partner is... We practice together by...")}
        };
        for (const QVariantList& row : rows) {
            QSqlQuery query(m_db);
            query.prepare(QStringLiteral(R"(
INSERT INTO local_speaking_task(title, level, prompt, reference_text, last_mode, last_score)
VALUES (?, ?, ?, ?, '', 0)
)"));
            for (const QVariant& value : row) {
                query.addBindValue(value);
            }
            if (!query.exec()) {
                if (errorMessage != nullptr) {
                    *errorMessage = query.lastError().text();
                }
                return false;
            }
        }
    }

    if (tableIsEmpty(QStringLiteral("local_community_item"))) {
        const QVariantList rows[] = {
            {QStringLiteral("晨读打卡小组"), QStringLiteral("CUSTOM_GROUP"), QStringLiteral("ACTIVE")},
            {QStringLiteral("同桌：Alex"), QStringLiteral("PARTNER"), QStringLiteral("ACTIVE")}
        };
        for (const QVariantList& row : rows) {
            QSqlQuery query(m_db);
            query.prepare(QStringLiteral("INSERT INTO local_community_item(name, kind, status) VALUES (?, ?, ?)"));
            query.addBindValue(row[0]);
            query.addBindValue(row[1]);
            query.addBindValue(row[2]);
            if (!query.exec()) {
                if (errorMessage != nullptr) {
                    *errorMessage = query.lastError().text();
                }
                return false;
            }
        }
    }

    if (tableIsEmpty(QStringLiteral("local_user_word_plan"))) {
        savePlan(1, 20, 10);
    }

    return true;
}

bool LocalStore::execute(const QString& sql, QString* errorMessage) const
{
    QSqlQuery query(m_db);
    if (!query.exec(sql)) {
        if (errorMessage != nullptr) {
            *errorMessage = query.lastError().text();
        }
        return false;
    }
    return true;
}

bool LocalStore::tableIsEmpty(const QString& tableName) const
{
    QSqlQuery query(m_db);
    query.exec(QStringLiteral("SELECT COUNT(*) FROM %1").arg(tableName));
    return query.next() && query.value(0).toInt() == 0;
}

bool LocalStore::enqueueOutbox(const QString& entityType, const QString& operation, const QString& payloadJson)
{
    if (!isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
INSERT INTO local_sync_outbox(entity_type, operation, payload_json, status, created_at)
VALUES (?, ?, ?, 'PENDING', ?)
)"));
    query.addBindValue(entityType);
    query.addBindValue(operation);
    query.addBindValue(payloadJson);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    return query.exec();
}

bool LocalStore::incrementTodaySummary(int newWords,
                                       int reviewWords,
                                       bool correct,
                                       int studySeconds,
                                       int scoreDelta,
                                       int completedTasks)
{
    if (!isOpen()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(R"(
INSERT INTO local_daily_summary(
    study_date, new_words, review_words, correct_count, wrong_count,
    study_seconds, score_delta, completed_tasks
) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
ON CONFLICT(study_date) DO UPDATE SET
    new_words = new_words + excluded.new_words,
    review_words = review_words + excluded.review_words,
    correct_count = correct_count + excluded.correct_count,
    wrong_count = wrong_count + excluded.wrong_count,
    study_seconds = study_seconds + excluded.study_seconds,
    score_delta = score_delta + excluded.score_delta,
    completed_tasks = completed_tasks + excluded.completed_tasks
)"));
    query.addBindValue(todayIso());
    query.addBindValue(newWords);
    query.addBindValue(reviewWords);
    query.addBindValue(correct ? 1 : 0);
    query.addBindValue(correct ? 0 : 1);
    query.addBindValue(studySeconds);
    query.addBindValue(scoreDelta);
    query.addBindValue(completedTasks);
    return query.exec();
}

QString LocalStore::createClientEventId() const
{
    return QStringLiteral("qt-%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}

QString LocalStore::todayIso() const
{
    return QDate::currentDate().toString(Qt::ISODate);
}
