#pragma once

#include <QMainWindow>

#include "ApiClient.h"

class QLabel;
class QRadioButton;
class QPushButton;

class EnglishLearner : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnglishLearner(QWidget* parent = nullptr);
    ~EnglishLearner() override;

private slots:
    void onSubmit();
    void onShowRanking();

private:
    void onLoginSuccess(const UserProfile& user);
    void loadWord();
    void updateScoreDisplay();
    void clearSelection();

private:
    QLabel* labelWelcome;
    QLabel* labelScore;
    QLabel* labelWord;
    QRadioButton* radioButtons[4];
    QPushButton* btnSubmit;
    QPushButton* btnRanking;
    QLabel* labelFeedback;

    ApiClient m_apiClient;
    UserProfile m_currentUser;
    QuizQuestion m_currentQuestion;
    QStringList m_currentOptions;
};
