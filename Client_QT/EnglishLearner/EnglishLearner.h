#pragma once

#include <QMainWindow>

#include "ApiClient.h"

class QFrame;
class QLabel;
class QProgressBar;
class QRadioButton;
class QPushButton;
class QTabWidget;
class QWidget;

class EnglishLearner : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnglishLearner(QWidget* parent = nullptr);
    ~EnglishLearner() override;

private slots:
    void onSubmit();
    void onShowRanking();
    void onOpenWordBook();
    void onOpenStudyStatus();

private:
    void applyTheme();
    void updateDailyVisual();
    QString generateDailyWord() const;
    QString generateDailyMeaning() const;
    void updateDailyProgress(int delta);
    void onLoginSuccess(const UserProfile& user);
    void loadWord();
    void updateScoreDisplay();
    void clearSelection();
    QWidget* createPlaceholderModule(const QString& moduleName, const QString& description);
    void setFeedbackText(const QString& text, bool positive);
    QString toDisplayName(const QString& rawUsername) const;

private:
    QLabel* labelAppName;
    QLabel* labelWelcome;
    QLabel* labelScore;

    QFrame* heroFrame;
    QLabel* labelDate;
    QLabel* labelDailyWord;
    QLabel* labelDailyMeaning;
    QPushButton* btnWordBook;
    QPushButton* btnStudyStatus;

    QTabWidget* navTabs;

    QLabel* labelWord;
    QRadioButton* radioButtons[4];
    QPushButton* btnSubmit;
    QPushButton* btnRanking;
    QLabel* labelFeedback;

    QProgressBar* progressDaily;
    QLabel* labelProgress;
    QLabel* labelGoal;

    ApiClient m_apiClient;
    UserProfile m_currentUser;
    QuizQuestion m_currentQuestion;
    QStringList m_currentOptions;

    int m_dailyGoal = 20;
    int m_dailyProgress = 0;
};
