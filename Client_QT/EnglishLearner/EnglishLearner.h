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
class QResizeEvent;
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

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onNavigate();
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
    enum PageIndex {
        PageToday = 0,
        PageWords,
        PagePractice,
        PageContent,
        PageReading,
        PageListeningSpeaking,
        PageMine
    };

    void buildUi();
    QWidget* createHeader(QWidget* parent);
    QWidget* createSidebar(QWidget* parent);
    QWidget* createBottomNav(QWidget* parent);
    QWidget* createStatusPanel(QWidget* parent);
    QPushButton* createNavButton(const QString& text, int pageIndex, QWidget* parent) const;
    QWidget* createTodayModule(QWidget* parent);
    QWidget* createWordModule(QWidget* parent);
    QWidget* createWordHomePage(QWidget* parent);
    QWidget* createWordPracticePage(QWidget* parent, const QString& modeName);
    QWidget* createPracticeModule(QWidget* parent);
    QWidget* createContentModule(QWidget* parent);
    QWidget* createReadingModule(QWidget* parent);
    QWidget* createListeningSpeakingModule(QWidget* parent);
    QWidget* createMineModule(QWidget* parent);

    QFrame* createCard(QWidget* parent, const QString& objectName = QString()) const;
    QLabel* createSectionTitle(const QString& text, QWidget* parent) const;
    QPushButton* createActionButton(const QString& text, const QString& objectName, QWidget* parent) const;

    void applyTheme();
    void applyAdaptiveLayout();
    void updateDailyVisual();
    void switchPage(int pageIndex);
    void updateNavigationState();
    void updateStatusPanel();
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
    QString todaySummaryText() const;
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

    QFrame* sidebarPanel = nullptr;
    QFrame* bottomNavBar = nullptr;
    QFrame* statusPanel = nullptr;
    QStackedWidget* pageStack = nullptr;
    QList<QPushButton*> sideNavButtons;
    QList<QPushButton*> bottomNavButtons;
    int currentPageIndex = PageToday;
    bool compactMode = false;
    QTabWidget* navTabs = nullptr;

    QLabel* labelTodayDate = nullptr;
    QLabel* labelTodayFocus = nullptr;
    QLabel* labelTodayTaskSummary = nullptr;
    QLabel* labelTodayStatus = nullptr;
    QProgressBar* progressTodayCompletion = nullptr;
    QLabel* labelPanelToday = nullptr;
    QLabel* labelPanelPlan = nullptr;
    QLabel* labelPanelSync = nullptr;

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
