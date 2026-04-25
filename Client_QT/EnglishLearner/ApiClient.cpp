#include "ApiClient.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtGlobal>

ApiClient::ApiClient()
{
    setBaseUrl("http://localhost:8080");
}

void ApiClient::setBaseUrl(const QString& baseUrl)
{
    QString normalized = baseUrl.trimmed();
    if (normalized.isEmpty()) {
        normalized = "http://localhost:8080";
    }
    if (!normalized.startsWith("http://", Qt::CaseInsensitive)
        && !normalized.startsWith("https://", Qt::CaseInsensitive)) {
        normalized.prepend("http://");
    }

    QUrl parsed(normalized);
    if (!parsed.isValid() || parsed.host().isEmpty()) {
        m_baseUrl = QUrl("http://localhost:8080");
        return;
    }

    if (parsed.path() == "/") {
        parsed.setPath(QString());
    } else if (parsed.path().endsWith('/')) {
        QString path = parsed.path();
        path.chop(1);
        parsed.setPath(path);
    }

    m_baseUrl = parsed;
}

QString ApiClient::baseUrl() const
{
    return m_baseUrl.toString();
}

void ApiClient::setToken(const QString& token)
{
    m_token = token.trimmed();
}

QString ApiClient::token() const
{
    return m_token;
}

bool ApiClient::registerUser(const QString& username, const QString& password, QString* errorMessage)
{
    const QString account = username.trimmed();
    QJsonObject body{
        {"username", account},
        {"password", password}
    };
    if (account.contains('@')) {
        body.insert("email", account);
    }
    const HttpResult result = sendJsonRequest("/api/v1/auth/register", "POST", body, false);

    QJsonValue data;
    return parseApiEnvelope(result, &data, errorMessage);
}

bool ApiClient::login(const QString& username, const QString& password,
                      UserProfile* user, QString* token, QString* errorMessage)
{
    const QString account = username.trimmed();
    const QJsonObject body{
        {"account", account},
        {"username", account},
        {"password", password}
    };
    const HttpResult result = sendJsonRequest("/api/v1/auth/login/password", "POST", body, false);

    QJsonValue dataValue;
    if (!parseApiEnvelope(result, &dataValue, errorMessage)) {
        return false;
    }
    if (!dataValue.isObject()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("\u767b\u5f55\u54cd\u5e94\u683c\u5f0f\u9519\u8bef");
        }
        return false;
    }

    const QJsonObject data = dataValue.toObject();
    const QString jwtToken = data.value("token").toString().trimmed();
    if (jwtToken.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("\u767b\u5f55\u6210\u529f\u4f46\u7f3a\u5c11token");
        }
        return false;
    }

    m_token = jwtToken;
    if (token != nullptr) {
        *token = jwtToken;
    }

    const UserProfile parsedUser = parseUserProfile(data.value("user").toObject());
    if (parsedUser.userId <= 0 || parsedUser.username.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("\u767b\u5f55\u54cd\u5e94\u7f3a\u5c11\u7528\u6237\u4fe1\u606f");
        }
        return false;
    }

    if (user != nullptr) {
        *user = parsedUser;
    }
    return true;
}

bool ApiClient::fetchCurrentUser(UserProfile* user, QString* errorMessage)
{
    const HttpResult result = sendJsonRequest("/api/v1/users/me", "GET", QJsonObject(), true);

    QJsonValue dataValue;
    if (!parseApiEnvelope(result, &dataValue, errorMessage)) {
        return false;
    }
    if (!dataValue.isObject()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("\u7528\u6237\u4fe1\u606f\u54cd\u5e94\u683c\u5f0f\u9519\u8bef");
        }
        return false;
    }

    const UserProfile parsedUser = parseUserProfile(dataValue.toObject());
    if (parsedUser.userId <= 0 || parsedUser.username.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("\u7528\u6237\u4fe1\u606f\u4e0d\u5b8c\u6574");
        }
        return false;
    }

    if (user != nullptr) {
        *user = parsedUser;
    }
    return true;
}

bool ApiClient::fetchNextQuestion(QuizQuestion* question, QString* errorMessage)
{
    const HttpResult result = sendJsonRequest("/api/v1/quiz/next", "GET", QJsonObject(), true);

    QJsonValue dataValue;
    if (!parseApiEnvelope(result, &dataValue, errorMessage)) {
        return false;
    }
    if (!dataValue.isObject()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("\u9898\u76ee\u54cd\u5e94\u683c\u5f0f\u9519\u8bef");
        }
        return false;
    }

    const QJsonObject data = dataValue.toObject();
    QuizQuestion parsed;
    parsed.wordId = static_cast<qint64>(data.value("wordId").toDouble());
    parsed.word = data.value("word").toString();
    parsed.phonetic = data.value("phonetic").toString();
    parsed.translation = data.value("translation").toString();
    parsed.exampleSentence = data.value("exampleSentence").toString();
    parsed.correctAnswer = data.value("correctAnswer").toString();

    const QJsonArray options = data.value("options").toArray();
    for (const QJsonValue& value : options) {
        parsed.options.append(value.toString());
    }

    if (parsed.wordId <= 0 || parsed.word.isEmpty() || parsed.options.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("\u9898\u76ee\u4fe1\u606f\u4e0d\u5b8c\u6574");
        }
        return false;
    }

    if (question != nullptr) {
        *question = parsed;
    }
    return true;
}

bool ApiClient::submitAnswer(qint64 wordId, const QString& selectedAnswer,
                             SubmitAnswerResult* result, QString* errorMessage)
{
    const QJsonObject body{
        {"wordId", static_cast<double>(wordId)},
        {"selectedAnswer", selectedAnswer}
    };

    const HttpResult response = sendJsonRequest("/api/v1/quiz/submit", "POST", body, true);

    QJsonValue dataValue;
    if (!parseApiEnvelope(response, &dataValue, errorMessage)) {
        return false;
    }
    if (!dataValue.isObject()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("\u7b54\u6848\u63d0\u4ea4\u54cd\u5e94\u683c\u5f0f\u9519\u8bef");
        }
        return false;
    }

    const QJsonObject data = dataValue.toObject();
    SubmitAnswerResult parsed;
    parsed.correct = data.value("correct").toBool(false);
    parsed.correctAnswer = data.value("correctAnswer").toString();
    parsed.scoreDelta = data.value("scoreDelta").toInt(0);
    parsed.totalScore = data.value("totalScore").toInt(0);

    if (result != nullptr) {
        *result = parsed;
    }
    return true;
}

bool ApiClient::fetchRanking(int limit, QList<RankingItem>* rankings, QString* errorMessage)
{
    const int safeLimit = qBound(1, limit, 100);
    const QString path = QString("/api/v1/ranking/top?limit=%1").arg(safeLimit);
    const HttpResult result = sendJsonRequest(path, "GET", QJsonObject(), false);

    QJsonValue dataValue;
    if (!parseApiEnvelope(result, &dataValue, errorMessage)) {
        return false;
    }
    if (!dataValue.isArray()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("\u6392\u884c\u699c\u54cd\u5e94\u683c\u5f0f\u9519\u8bef");
        }
        return false;
    }

    QList<RankingItem> parsed;
    const QJsonArray array = dataValue.toArray();
    parsed.reserve(array.size());

    for (const QJsonValue& value : array) {
        const QJsonObject object = value.toObject();
        RankingItem item;
        item.rank = object.value("rank").toInt();
        item.userId = static_cast<qint64>(object.value("userId").toDouble());
        item.username = object.value("username").toString();
        item.totalScore = object.value("totalScore").toInt();
        parsed.append(item);
    }

    if (rankings != nullptr) {
        *rankings = parsed;
    }
    return true;
}

QUrl ApiClient::buildUrl(const QString& path) const
{
    QString normalizedPath = path.trimmed();
    if (normalizedPath.isEmpty()) {
        normalizedPath = "/";
    }
    if (!normalizedPath.startsWith('/')) {
        normalizedPath.prepend('/');
    }

    return m_baseUrl.resolved(QUrl(normalizedPath));
}

ApiClient::HttpResult ApiClient::sendJsonRequest(const QString& path,
                                                 const QByteArray& method,
                                                 const QJsonObject& body,
                                                 bool authorized)
{
    QNetworkRequest request(buildUrl(path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");

    if (authorized && !m_token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_token).toUtf8());
    }

    QNetworkReply* reply = nullptr;
    if (method == "GET") {
        reply = m_networkManager.get(request);
    } else if (method == "POST") {
        const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
        reply = m_networkManager.post(request, payload);
    } else {
        const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
        reply = m_networkManager.sendCustomRequest(request, method, payload);
    }

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    HttpResult result;
    result.statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    result.rawBody = QString::fromUtf8(reply->readAll());
    result.networkError = reply->error() == QNetworkReply::NoError ? QString() : reply->errorString();

    QJsonParseError parseError;
    const QJsonDocument json = QJsonDocument::fromJson(result.rawBody.toUtf8(), &parseError);
    if (parseError.error == QJsonParseError::NoError && json.isObject()) {
        result.root = json.object();
        result.hasJson = true;
    }

    reply->deleteLater();
    return result;
}

bool ApiClient::parseApiEnvelope(const HttpResult& result, QJsonValue* data, QString* errorMessage) const
{
    if (!result.hasJson) {
        if (errorMessage != nullptr) {
            if (!result.networkError.isEmpty()) {
                *errorMessage = result.networkError;
            } else if (!result.rawBody.isEmpty()) {
                *errorMessage = QStringLiteral("\u670d\u52a1\u5668\u8fd4\u56de\u975eJSON\u5185\u5bb9\uff08HTTP %1\uff09")
                                    .arg(result.statusCode);
            } else {
                *errorMessage = QStringLiteral("\u8bf7\u6c42\u5931\u8d25\uff08HTTP %1\uff09").arg(result.statusCode);
            }
        }
        return false;
    }

    const bool httpOk = result.statusCode >= 200 && result.statusCode < 300;
    const bool success = result.root.value("success").toBool(false);
    const QString message = result.root.value("message").toString();

    if (!httpOk || !success) {
        if (errorMessage != nullptr) {
            if (!message.isEmpty()) {
                *errorMessage = message;
            } else if (!result.networkError.isEmpty()) {
                *errorMessage = result.networkError;
            } else {
                *errorMessage = QStringLiteral("\u8bf7\u6c42\u5931\u8d25\uff08HTTP %1\uff09").arg(result.statusCode);
            }
        }
        return false;
    }

    if (data != nullptr) {
        *data = result.root.value("data");
    }
    return true;
}

UserProfile ApiClient::parseUserProfile(const QJsonObject& object)
{
    UserProfile user;
    user.userId = static_cast<qint64>(object.value("userId").toDouble());
    user.username = object.value("username").toString();
    user.email = object.value("email").toString();
    user.displayName = object.value("displayName").toString();
    user.avatarUrl = object.value("avatarUrl").toString();
    user.status = object.value("status").toString();
    user.emailVerified = object.value("emailVerified").toBool(false);
    user.totalScore = object.value("totalScore").toInt(0);
    user.studyDays = object.value("studyDays").toInt(0);
    return user;
}
