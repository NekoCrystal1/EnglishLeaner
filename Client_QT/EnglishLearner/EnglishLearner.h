#pragma once

#include <QMainWindow>

#include "ApiClient.h"

class QFrame;
class QLabel;
class QProgressBar;
class QPushButton;
class QStackedWidget;
class QTabWidget;
class QWidget;

class EnglishLearner : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnglishLearner(QWidget* parent = nullptr);
    ~EnglishLearner() override;

private slots:
    void onOpenWordBook();
    void onOpenStudyStatus();
    void onStartLearn();
    void onStartReview();
    void onBackToWordHome();

private:
    void applyTheme();
    void updateDailyVisual();
    QString generateDailyWord() const;
    QString generateDailyMeaning() const;
    void updateDailyProgress(int delta);
    void onLoginSuccess(const UserProfile& user);
    void updateScoreDisplay();
    QWidget* createWordModePage(const QString& moduleName,
                                const QString& description,
                                QPushButton** backButton);
    QWidget* createPlaceholderModule(const QString& moduleName, const QString& description);
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

    QStackedWidget* wordStack;
    QWidget* wordHomePage;
    QWidget* wordLearnPage;
    QWidget* wordReviewPage;
    QPushButton* btnStartLearn;
    QPushButton* btnStartReview;
    QPushButton* btnBackFromLearn;
    QPushButton* btnBackFromReview;

    QProgressBar* progressDaily;
    QLabel* labelProgress;
    QLabel* labelGoal;

    ApiClient m_apiClient;
    UserProfile m_currentUser;

    int m_dailyGoal = 20;
    int m_dailyProgress = 0;
};
