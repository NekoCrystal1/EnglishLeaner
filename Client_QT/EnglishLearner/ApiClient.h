#pragma once

#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QNetworkAccessManager>
#include <QString>
#include <QStringList>
#include <QUrl>

struct UserProfile {
    qint64 userId = 0;
    QString username;
    QString email;
    QString displayName;
    QString avatarUrl;
    QString status;
    bool emailVerified = false;
    int totalScore = 0;
    int studyDays = 0;
};

struct QuizQuestion {
    qint64 wordId = 0;
    QString word;
    QString phonetic;
    QString translation;
    QString exampleSentence;
    QString correctAnswer;
    QStringList options;
};

struct SubmitAnswerResult {
    bool correct = false;
    QString correctAnswer;
    int scoreDelta = 0;
    int totalScore = 0;
};

struct RankingItem {
    int rank = 0;
    qint64 userId = 0;
    QString username;
    int totalScore = 0;
};

class ApiClient
{
public:
    ApiClient();

    void setBaseUrl(const QString& baseUrl);
    QString baseUrl() const;

    void setToken(const QString& token);
    QString token() const;

    bool registerUser(const QString& username, const QString& password, QString* errorMessage);
    bool login(const QString& username, const QString& password,
               UserProfile* user, QString* token, QString* errorMessage);
    bool fetchCurrentUser(UserProfile* user, QString* errorMessage);
    bool fetchNextQuestion(QuizQuestion* question, QString* errorMessage);
    bool submitAnswer(qint64 wordId, const QString& selectedAnswer,
                      SubmitAnswerResult* result, QString* errorMessage);
    bool fetchRanking(int limit, QList<RankingItem>* rankings, QString* errorMessage);

private:
    struct HttpResult {
        int statusCode = 0;
        QJsonObject root;
        QString rawBody;
        QString networkError;
        bool hasJson = false;
    };

    QNetworkAccessManager m_networkManager;
    QUrl m_baseUrl;
    QString m_token;

    QUrl buildUrl(const QString& path) const;
    HttpResult sendJsonRequest(const QString& path,
                               const QByteArray& method,
                               const QJsonObject& body,
                               bool authorized);
    bool parseApiEnvelope(const HttpResult& result, QJsonValue* data, QString* errorMessage) const;
    static UserProfile parseUserProfile(const QJsonObject& object);
};
