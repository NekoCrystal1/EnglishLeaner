#pragma once

#include <QMainWindow>

#include "ApiClient.h"
#include "LocalStore.h"

class QComboBox;
class QFrame;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPlainTextEdit;
class QProgressBar;
class QPushButton;
class QSlider;
class QSpinBox;
class QStackedWidget;
class QTabWidget;
class QTextBrowser;
class QWidget;

class EnglishLearner : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnglishLearner(QWidget* parent = nullptr);
    ~EnglishLearner() override;

private slots:
    void onOpenStudyStatus();
    void onStartLearn();
    void onStartReview();
    void onBackToWordHome();
    void onSavePlan();
    void onLoadNextQuestion();
    void onAnswerOption();
    void onReadingSelectionChanged();
    void onMarkReadingComplete();
    void onListeningSelectionChanged();
    void onMarkListeningComplete();
    void onSpeakingSelectionChanged();
    void onEvaluateSpeaking();
    void onCheckIn();
    void onRefreshRanking();
    void onSyncNow();
    void onCreateSharePackage();
    void onImportLocalContent();
    void onCreateStudyGroup();

private:
    void buildUi();
    QWidget* createHeader(QWidget* parent);
    QWidget* createWordModule(QWidget* parent);
    QWidget* createWordHomePage(QWidget* parent);
    QWidget* createWordPracticePage(QWidget* parent, const QString& modeName);
    QWidget* createReadingModule(QWidget* parent);
    QWidget* createListeningSpeakingModule(QWidget* parent);
    QWidget* createMineModule(QWidget* parent);

    QFrame* createCard(QWidget* parent, const QString& objectName = QString()) const;
    QLabel* createSectionTitle(const QString& text, QWidget* parent) const;
    QPushButton* createActionButton(const QString& text, const QString& objectName, QWidget* parent) const;

    void applyTheme();
    void updateDailyVisual();
    void refreshAll();
    void refreshBooks();
    void refreshTodaySummary();
    void refreshReadingList();
    void refreshListeningList();
    void refreshSpeakingList();
    void refreshCommunityList();
    void refreshSyncState();
    void updateScoreDisplay();
    void updateWordProgressDisplay();
    void enterPracticeMode(const QString& activityType);
    void setCurrentQuestion(const QuizQuestion& question, bool fromServer);
    QuizQuestion fallbackQuestion() const;
    QString generateDailyWord() const;
    QString generateDailyMeaning() const;
    QString toDisplayName(const QString& rawUsername) const;
    QString syncStatusText() const;
    void onLoginSuccess(const UserProfile& user);

private:
    ApiClient m_apiClient;
    LocalStore m_store;
    UserProfile m_currentUser;
    QuizQuestion m_currentQuestion;
    QString m_currentActivityType = QStringLiteral("LEARN");
    bool m_currentQuestionFromServer = false;

    QLabel* labelAppName = nullptr;
    QLabel* labelWelcome = nullptr;
    QLabel* labelScore = nullptr;
    QLabel* labelSyncStatus = nullptr;
    QPushButton* btnSyncNow = nullptr;

    QTabWidget* navTabs = nullptr;

    QFrame* heroFrame = nullptr;
    QLabel* labelDate = nullptr;
    QLabel* labelDailyWord = nullptr;
    QLabel* labelDailyMeaning = nullptr;
    QLabel* labelTodayStats = nullptr;
    QLabel* labelWordProgress = nullptr;
    QProgressBar* progressDaily = nullptr;

    QStackedWidget* wordStack = nullptr;
    QWidget* wordHomePage = nullptr;
    QWidget* wordLearnPage = nullptr;
    QWidget* wordReviewPage = nullptr;
    QComboBox* comboBooks = nullptr;
    QSpinBox* spinDailyNew = nullptr;
    QSpinBox* spinDailyReview = nullptr;
    QLabel* labelQuestionWord = nullptr;
    QLabel* labelQuestionMeta = nullptr;
    QLabel* labelQuestionExample = nullptr;
    QLabel* labelAnswerFeedback = nullptr;
    QList<QPushButton*> optionButtons;

    QListWidget* listReading = nullptr;
    QTextBrowser* textReadingContent = nullptr;
    QLabel* labelReadingMeta = nullptr;
    QProgressBar* progressReading = nullptr;

    QListWidget* listListening = nullptr;
    QTextBrowser* textListeningTranscript = nullptr;
    QLabel* labelListeningMeta = nullptr;
    QProgressBar* progressListening = nullptr;
    QListWidget* listSpeaking = nullptr;
    QTextBrowser* textSpeakingPrompt = nullptr;
    QLabel* labelSpeakingMeta = nullptr;

    QLabel* labelMineStats = nullptr;
    QListWidget* listRanking = nullptr;
    QListWidget* listCommunity = nullptr;
    QLabel* labelLocalStore = nullptr;
    QLabel* labelOutbox = nullptr;
};
