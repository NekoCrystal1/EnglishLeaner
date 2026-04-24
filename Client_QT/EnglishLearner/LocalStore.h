#pragma once

#include "ApiClient.h"

#include <QDate>
#include <QList>
#include <QSqlDatabase>
#include <QString>

struct LocalBook {
    qint64 localId = 0;
    QString title;
    QString level;
    QString sourceType;
    QString storageMode;
    int itemCount = 0;
    int learnedCount = 0;
    int dueCount = 0;
    int masteredCount = 0;
};

struct LocalStudySummary {
    int newWords = 0;
    int reviewWords = 0;
    int correctCount = 0;
    int wrongCount = 0;
    int studySeconds = 0;
    int scoreDelta = 0;
    int completedTasks = 0;
};

struct LocalReadingItem {
    qint64 localId = 0;
    QString title;
    QString level;
    QString sourceType;
    int wordCount = 0;
    int estimatedMinutes = 0;
    int progressPercent = 0;
    QString content;
};

struct LocalListeningItem {
    qint64 localId = 0;
    QString title;
    QString level;
    int durationSeconds = 0;
    int listenSeconds = 0;
    int completedCount = 0;
    QString transcript;
};

struct LocalSpeakingTask {
    qint64 localId = 0;
    QString title;
    QString level;
    QString prompt;
    QString referenceText;
    QString lastMode;
    int lastScore = 0;
};

struct LocalCommunityItem {
    qint64 localId = 0;
    QString name;
    QString kind;
    QString status;
};

class LocalStore
{
public:
    LocalStore();
    ~LocalStore();

    bool openForUser(const UserProfile& user, QString* errorMessage);
    bool isOpen() const;
    QString databasePath() const;
    QString deviceUuid();

    bool saveUserProfile(const UserProfile& user);

    QList<LocalBook> books() const;
    QList<LocalReadingItem> readingItems() const;
    QList<LocalListeningItem> listeningItems() const;
    QList<LocalSpeakingTask> speakingTasks() const;
    QList<LocalCommunityItem> communityItems() const;
    LocalStudySummary todaySummary() const;

    bool savePlan(qint64 bookId, int dailyNewCount, int dailyReviewCount);
    bool recordWordStudy(qint64 userId,
                         const QuizQuestion& question,
                         const QString& selectedAnswer,
                         bool correct,
                         int scoreDelta,
                         const QString& activityType);
    bool completeReading(qint64 readingId);
    bool completeListening(qint64 listeningId);
    bool saveSpeakingAttempt(qint64 taskId, int score, const QString& mode);
    bool checkIn(qint64 userId, int rewardScore);
    bool createShareRecord(const QString& targetType, const QString& targetName);
    bool createStudyGroup(const QString& name);
    bool markAllSynced();

    int pendingOutboxCount() const;
    int unsyncedEventCount() const;

private:
    bool initializeSchema(QString* errorMessage);
    bool seedDefaults(QString* errorMessage);
    bool execute(const QString& sql, QString* errorMessage = nullptr) const;
    bool tableIsEmpty(const QString& tableName) const;
    bool enqueueOutbox(const QString& entityType, const QString& operation, const QString& payloadJson);
    bool incrementTodaySummary(int newWords,
                               int reviewWords,
                               bool correct,
                               int studySeconds,
                               int scoreDelta,
                               int completedTasks);
    QString createClientEventId() const;
    QString todayIso() const;

private:
    QSqlDatabase m_db;
    QString m_connectionName;
    QString m_databasePath;
    qint64 m_userId = 0;
};
