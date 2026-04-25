#include "EnglishLearner.h"

#include "LoginDialog.h"

#include <QComboBox>
#include <QDate>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPalette>
#include <QPair>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QVBoxLayout>
#include <QtGlobal>

EnglishLearner::EnglishLearner(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("LingoPulse 客户端"));
    resize(1120, 760);
    setMinimumSize(390, 620);

    buildUi();
    applyTheme();
    updateDailyVisual();

    LoginDialog loginDlg(&m_apiClient, this);
    if (loginDlg.exec() != QDialog::Accepted) {
        QTimer::singleShot(0, this, &QWidget::close);
        return;
    }

    onLoginSuccess(loginDlg.loggedInUser());
}

EnglishLearner::~EnglishLearner() = default;

void EnglishLearner::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    applyAdaptiveLayout();
}

void EnglishLearner::buildUi()
{
    QWidget* rootPanel = new QWidget(this);
    rootPanel->setObjectName("rootPanel");
    setCentralWidget(rootPanel);

    QVBoxLayout* rootLayout = new QVBoxLayout(rootPanel);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(14);

    rootLayout->addWidget(createHeader(rootPanel));

    QHBoxLayout* shellLayout = new QHBoxLayout();
    shellLayout->setContentsMargins(0, 0, 0, 0);
    shellLayout->setSpacing(14);

    sidebarPanel = qobject_cast<QFrame*>(createSidebar(rootPanel));
    shellLayout->addWidget(sidebarPanel);

    pageStack = new QStackedWidget(rootPanel);
    pageStack->setObjectName("pageStack");
    pageStack->addWidget(createTodayModule(pageStack));
    pageStack->addWidget(createWordModule(pageStack));
    pageStack->addWidget(createPracticeModule(pageStack));
    pageStack->addWidget(createContentModule(pageStack));
    pageStack->addWidget(createReadingModule(pageStack));
    pageStack->addWidget(createListeningSpeakingModule(pageStack));
    pageStack->addWidget(createMineModule(pageStack));
    shellLayout->addWidget(pageStack, 1);

    statusPanel = qobject_cast<QFrame*>(createStatusPanel(rootPanel));
    shellLayout->addWidget(statusPanel);

    rootLayout->addLayout(shellLayout, 1);

    bottomNavBar = qobject_cast<QFrame*>(createBottomNav(rootPanel));
    rootLayout->addWidget(bottomNavBar);

    switchPage(PageToday);
    applyAdaptiveLayout();
}

QWidget* EnglishLearner::createHeader(QWidget* parent)
{
    QFrame* headerCard = createCard(parent, QStringLiteral("headerCard"));
    QHBoxLayout* headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setContentsMargins(16, 12, 16, 12);
    headerLayout->setSpacing(12);

    labelAppName = new QLabel(QStringLiteral("LingoPulse"), headerCard);
    labelAppName->setObjectName("appName");
    labelWelcome = new QLabel(QStringLiteral("访客"), headerCard);
    labelWelcome->setObjectName("welcomeText");
    labelScore = new QLabel(QStringLiteral("积分：0"), headerCard);
    labelScore->setObjectName("scoreText");
    labelSyncStatus = new QLabel(QStringLiteral("本地库未打开"), headerCard);
    labelSyncStatus->setObjectName("syncText");
    btnSyncNow = createActionButton(QStringLiteral("同步"), QStringLiteral("ghostButton"), headerCard);

    headerLayout->addWidget(labelAppName);
    headerLayout->addStretch();
    headerLayout->addWidget(labelWelcome);
    headerLayout->addWidget(labelScore);
    headerLayout->addWidget(labelSyncStatus);
    headerLayout->addWidget(btnSyncNow);

    connect(btnSyncNow, &QPushButton::clicked, this, &EnglishLearner::onSyncNow);
    return headerCard;
}

QWidget* EnglishLearner::createSidebar(QWidget* parent)
{
    QFrame* panel = createCard(parent, QStringLiteral("sidebarPanel"));
    panel->setMinimumWidth(210);
    panel->setMaximumWidth(260);

    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(12, 14, 12, 14);
    layout->setSpacing(8);

    QLabel* learningGroup = new QLabel(QStringLiteral("学习"), panel);
    learningGroup->setObjectName("navGroupTitle");
    layout->addWidget(learningGroup);

    const QList<QPair<QString, int>> learningItems{
        {QStringLiteral("今日"), PageToday},
        {QStringLiteral("单词"), PageWords},
        {QStringLiteral("练习"), PagePractice}
    };
    for (const auto& item : learningItems) {
        QPushButton* button = createNavButton(item.first, item.second, panel);
        sideNavButtons.append(button);
        layout->addWidget(button);
        connect(button, &QPushButton::clicked, this, &EnglishLearner::onNavigate);
    }

    QLabel* contentGroup = new QLabel(QStringLiteral("内容"), panel);
    contentGroup->setObjectName("navGroupTitle");
    layout->addWidget(contentGroup);

    const QList<QPair<QString, int>> contentItems{
        {QStringLiteral("阅读"), PageReading},
        {QStringLiteral("听说"), PageListeningSpeaking}
    };
    for (const auto& item : contentItems) {
        QPushButton* button = createNavButton(item.first, item.second, panel);
        sideNavButtons.append(button);
        layout->addWidget(button);
        connect(button, &QPushButton::clicked, this, &EnglishLearner::onNavigate);
    }

    QLabel* personalGroup = new QLabel(QStringLiteral("个人"), panel);
    personalGroup->setObjectName("navGroupTitle");
    layout->addWidget(personalGroup);

    QPushButton* mineButton = createNavButton(QStringLiteral("我的"), PageMine, panel);
    sideNavButtons.append(mineButton);
    layout->addWidget(mineButton);
    connect(mineButton, &QPushButton::clicked, this, &EnglishLearner::onNavigate);

    layout->addStretch();
    return panel;
}

QWidget* EnglishLearner::createBottomNav(QWidget* parent)
{
    QFrame* panel = createCard(parent, QStringLiteral("bottomNavBar"));
    QHBoxLayout* layout = new QHBoxLayout(panel);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    const QList<QPair<QString, int>> items{
        {QStringLiteral("今日"), PageToday},
        {QStringLiteral("单词"), PageWords},
        {QStringLiteral("练习"), PagePractice},
        {QStringLiteral("内容"), PageContent},
        {QStringLiteral("我的"), PageMine}
    };

    for (const auto& item : items) {
        QPushButton* button = createNavButton(item.first, item.second, panel);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        bottomNavButtons.append(button);
        layout->addWidget(button);
        connect(button, &QPushButton::clicked, this, &EnglishLearner::onNavigate);
    }

    return panel;
}

QWidget* EnglishLearner::createStatusPanel(QWidget* parent)
{
    QFrame* panel = createCard(parent, QStringLiteral("statusPanel"));
    panel->setMinimumWidth(250);
    panel->setMaximumWidth(300);

    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);

    layout->addWidget(createSectionTitle(QStringLiteral("今日状态"), panel));
    labelPanelToday = new QLabel(panel);
    labelPanelToday->setObjectName("moduleDescription");
    labelPanelToday->setWordWrap(true);
    layout->addWidget(labelPanelToday);

    layout->addWidget(createSectionTitle(QStringLiteral("当前计划"), panel));
    labelPanelPlan = new QLabel(panel);
    labelPanelPlan->setObjectName("moduleDescription");
    labelPanelPlan->setWordWrap(true);
    layout->addWidget(labelPanelPlan);

    layout->addWidget(createSectionTitle(QStringLiteral("同步"), panel));
    labelPanelSync = new QLabel(panel);
    labelPanelSync->setObjectName("moduleDescription");
    labelPanelSync->setWordWrap(true);
    layout->addWidget(labelPanelSync);

    QPushButton* syncButton = createActionButton(QStringLiteral("立即同步"), QStringLiteral("ghostButton"), panel);
    layout->addWidget(syncButton);
    connect(syncButton, &QPushButton::clicked, this, &EnglishLearner::onSyncNow);

    layout->addStretch();
    return panel;
}

QPushButton* EnglishLearner::createNavButton(const QString& text, int pageIndex, QWidget* parent) const
{
    QPushButton* button = new QPushButton(text, parent);
    button->setObjectName("navButton");
    button->setCheckable(true);
    button->setProperty("pageIndex", pageIndex);
    return button;
}

QWidget* EnglishLearner::createTodayModule(QWidget* parent)
{
    QScrollArea* scroll = new QScrollArea(parent);
    scroll->setObjectName("plainScroll");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget* page = new QWidget(scroll);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);

    QFrame* hero = createCard(page, QStringLiteral("todayHeroCard"));
    QVBoxLayout* heroLayout = new QVBoxLayout(hero);
    heroLayout->setContentsMargins(18, 18, 18, 18);
    heroLayout->setSpacing(10);

    labelTodayDate = new QLabel(hero);
    labelTodayDate->setObjectName("heroDate");
    labelTodayFocus = new QLabel(hero);
    labelTodayFocus->setObjectName("dailyWord");
    labelTodayFocus->setWordWrap(true);
    labelTodayTaskSummary = new QLabel(hero);
    labelTodayTaskSummary->setObjectName("dailyMeaning");
    labelTodayTaskSummary->setWordWrap(true);

    heroLayout->addWidget(labelTodayDate);
    heroLayout->addWidget(labelTodayFocus);
    heroLayout->addWidget(labelTodayTaskSummary);
    layout->addWidget(hero);

    QFrame* taskCard = createCard(page, QStringLiteral("entryActionCard"));
    QGridLayout* taskLayout = new QGridLayout(taskCard);
    taskLayout->setContentsMargins(16, 16, 16, 16);
    taskLayout->setHorizontalSpacing(10);
    taskLayout->setVerticalSpacing(10);
    taskLayout->addWidget(createSectionTitle(QStringLiteral("今日任务队列"), taskCard), 0, 0, 1, 3);

    progressTodayCompletion = new QProgressBar(taskCard);
    progressTodayCompletion->setRange(0, 100);
    progressTodayCompletion->setFormat(QStringLiteral("完成度 %p%"));
    taskLayout->addWidget(progressTodayCompletion, 1, 0, 1, 3);

    labelTodayStatus = new QLabel(taskCard);
    labelTodayStatus->setObjectName("moduleDescription");
    labelTodayStatus->setWordWrap(true);
    taskLayout->addWidget(labelTodayStatus, 2, 0, 1, 3);

    QPushButton* reviewButton = createActionButton(QStringLiteral("继续复习"), QStringLiteral("primaryButton"), taskCard);
    QPushButton* learnButton = createActionButton(QStringLiteral("开始新词"), QStringLiteral("secondaryButton"), taskCard);
    QPushButton* contentButton = createActionButton(QStringLiteral("内容训练"), QStringLiteral("ghostButton"), taskCard);
    taskLayout->addWidget(reviewButton, 3, 0);
    taskLayout->addWidget(learnButton, 3, 1);
    taskLayout->addWidget(contentButton, 3, 2);
    layout->addWidget(taskCard);

    QFrame* hintCard = createCard(page);
    QVBoxLayout* hintLayout = new QVBoxLayout(hintCard);
    hintLayout->setContentsMargins(16, 16, 16, 16);
    hintLayout->setSpacing(8);
    hintLayout->addWidget(createSectionTitle(QStringLiteral("学习节奏"), hintCard));
    QLabel* hint = new QLabel(
        QStringLiteral("优先完成到期复习，再引入新词；阅读、听力和口语任务会记录本地进度，离线时也能继续。"),
        hintCard);
    hint->setObjectName("moduleDescription");
    hint->setWordWrap(true);
    hintLayout->addWidget(hint);
    layout->addWidget(hintCard);

    layout->addStretch();
    scroll->setWidget(page);

    connect(reviewButton, &QPushButton::clicked, this, &EnglishLearner::onStartReview);
    connect(learnButton, &QPushButton::clicked, this, &EnglishLearner::onStartLearn);
    connect(contentButton, &QPushButton::clicked, this, [this]() { switchPage(PageContent); });
    return scroll;
}

QWidget* EnglishLearner::createWordModule(QWidget* parent)
{
    QWidget* wordModule = new QWidget(parent);
    QVBoxLayout* wordLayout = new QVBoxLayout(wordModule);
    wordLayout->setContentsMargins(14, 14, 14, 14);
    wordLayout->setSpacing(12);

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->addWidget(createSectionTitle(QStringLiteral("单词学习"), wordModule));
    titleLayout->addStretch();
    QPushButton* statusButton = createActionButton(QStringLiteral("学习状态"), QStringLiteral("ghostButton"), wordModule);
    titleLayout->addWidget(statusButton);
    wordLayout->addLayout(titleLayout);
    connect(statusButton, &QPushButton::clicked, this, &EnglishLearner::onOpenStudyStatus);

    heroFrame = createCard(wordModule, QStringLiteral("heroFrame"));
    QVBoxLayout* heroLayout = new QVBoxLayout(heroFrame);
    heroLayout->setContentsMargins(18, 16, 18, 16);
    heroLayout->setSpacing(10);

    labelDate = new QLabel(heroFrame);
    labelDate->setObjectName("heroDate");
    labelDailyWord = new QLabel(heroFrame);
    labelDailyWord->setObjectName("dailyWord");
    labelDailyMeaning = new QLabel(heroFrame);
    labelDailyMeaning->setObjectName("dailyMeaning");
    labelDailyMeaning->setWordWrap(true);

    heroLayout->addWidget(labelDate);
    heroLayout->addWidget(labelDailyWord);
    heroLayout->addWidget(labelDailyMeaning);
    wordLayout->addWidget(heroFrame);

    wordStack = new QStackedWidget(wordModule);
    wordHomePage = createWordHomePage(wordStack);
    wordLearnPage = createWordPracticePage(wordStack, QStringLiteral("单词练习"));
    wordReviewPage = wordLearnPage;
    wordStack->addWidget(wordHomePage);
    wordStack->addWidget(wordLearnPage);
    wordStack->setCurrentWidget(wordHomePage);
    wordLayout->addWidget(wordStack, 1);

    return wordModule;
}

QWidget* EnglishLearner::createWordHomePage(QWidget* parent)
{
    QWidget* page = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    QFrame* planCard = createCard(page, QStringLiteral("entryActionCard"));
    QGridLayout* planLayout = new QGridLayout(planCard);
    planLayout->setContentsMargins(16, 16, 16, 16);
    planLayout->setHorizontalSpacing(12);
    planLayout->setVerticalSpacing(10);

    planLayout->addWidget(createSectionTitle(QStringLiteral("当前词书与计划"), planCard), 0, 0, 1, 4);

    comboBooks = new QComboBox(planCard);
    comboBooks->setObjectName("field");
    spinDailyNew = new QSpinBox(planCard);
    spinDailyNew->setRange(1, 200);
    spinDailyNew->setValue(20);
    spinDailyNew->setSuffix(QStringLiteral(" 新词"));
    spinDailyReview = new QSpinBox(planCard);
    spinDailyReview->setRange(0, 300);
    spinDailyReview->setValue(10);
    spinDailyReview->setSuffix(QStringLiteral(" 复习"));

    QPushButton* savePlanButton = createActionButton(QStringLiteral("保存计划"), QStringLiteral("primaryButton"), planCard);
    QPushButton* importButton = createActionButton(QStringLiteral("导入本地内容"), QStringLiteral("secondaryButton"), planCard);

    planLayout->addWidget(new QLabel(QStringLiteral("词书"), planCard), 1, 0);
    planLayout->addWidget(comboBooks, 1, 1, 1, 3);
    planLayout->addWidget(new QLabel(QStringLiteral("每日目标"), planCard), 2, 0);
    planLayout->addWidget(spinDailyNew, 2, 1);
    planLayout->addWidget(spinDailyReview, 2, 2);
    planLayout->addWidget(savePlanButton, 2, 3);
    planLayout->addWidget(importButton, 3, 3);

    labelWordProgress = new QLabel(planCard);
    labelWordProgress->setObjectName("moduleDescription");
    labelWordProgress->setWordWrap(true);
    progressDaily = new QProgressBar(planCard);
    progressDaily->setRange(0, 100);
    progressDaily->setValue(0);
    progressDaily->setFormat("%p%");
    planLayout->addWidget(progressDaily, 3, 0, 1, 3);
    planLayout->addWidget(labelWordProgress, 4, 0, 1, 4);

    QHBoxLayout* actionLayout = new QHBoxLayout();
    QPushButton* learnButton = createActionButton(QStringLiteral("开始学习"), QStringLiteral("modeLearnButton"), page);
    QPushButton* reviewButton = createActionButton(QStringLiteral("开始复习"), QStringLiteral("modeReviewButton"), page);
    actionLayout->addWidget(learnButton);
    actionLayout->addWidget(reviewButton);

    QFrame* summaryCard = createCard(page);
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryCard);
    summaryLayout->setContentsMargins(16, 16, 16, 16);
    labelTodayStats = new QLabel(summaryCard);
    labelTodayStats->setObjectName("moduleDescription");
    labelTodayStats->setWordWrap(true);
    summaryLayout->addWidget(createSectionTitle(QStringLiteral("今日概览"), summaryCard));
    summaryLayout->addWidget(labelTodayStats);

    layout->addWidget(planCard);
    layout->addLayout(actionLayout);
    layout->addWidget(summaryCard);
    layout->addStretch();

    connect(savePlanButton, &QPushButton::clicked, this, &EnglishLearner::onSavePlan);
    connect(importButton, &QPushButton::clicked, this, &EnglishLearner::onImportLocalContent);
    connect(learnButton, &QPushButton::clicked, this, &EnglishLearner::onStartLearn);
    connect(reviewButton, &QPushButton::clicked, this, &EnglishLearner::onStartReview);
    return page;
}

QWidget* EnglishLearner::createWordPracticePage(QWidget* parent, const QString& modeName)
{
    QWidget* page = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    QFrame* card = createCard(page);
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 18, 18, 18);
    cardLayout->setSpacing(12);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(createSectionTitle(QStringLiteral("%1模式").arg(modeName), card));
    topLayout->addStretch();
    QPushButton* nextButton = createActionButton(QStringLiteral("换一题"), QStringLiteral("ghostButton"), card);
    QPushButton* backButton = createActionButton(QStringLiteral("返回"), QStringLiteral("secondaryButton"), card);
    topLayout->addWidget(nextButton);
    topLayout->addWidget(backButton);
    cardLayout->addLayout(topLayout);

    labelQuestionWord = new QLabel(QStringLiteral("Ready"), card);
    labelQuestionWord->setObjectName("questionWord");
    labelQuestionMeta = new QLabel(card);
    labelQuestionMeta->setObjectName("moduleDescription");
    labelQuestionMeta->setWordWrap(true);
    labelQuestionExample = new QLabel(card);
    labelQuestionExample->setObjectName("moduleDescription");
    labelQuestionExample->setWordWrap(true);
    labelAnswerFeedback = new QLabel(card);
    labelAnswerFeedback->setObjectName("answerFeedback");
    labelAnswerFeedback->setWordWrap(true);

    cardLayout->addWidget(labelQuestionWord);
    cardLayout->addWidget(labelQuestionMeta);
    cardLayout->addWidget(labelQuestionExample);

    QGridLayout* optionLayout = new QGridLayout();
    optionLayout->setSpacing(10);
    for (int i = 0; i < 4; ++i) {
        QPushButton* button = createActionButton(QStringLiteral("选项 %1").arg(i + 1), QStringLiteral("optionButton"), card);
        button->setProperty("optionIndex", i);
        optionButtons.append(button);
        optionLayout->addWidget(button, i / 2, i % 2);
        connect(button, &QPushButton::clicked, this, &EnglishLearner::onAnswerOption);
    }
    cardLayout->addLayout(optionLayout);
    cardLayout->addWidget(labelAnswerFeedback);

    layout->addWidget(card);
    layout->addStretch();

    connect(nextButton, &QPushButton::clicked, this, &EnglishLearner::onLoadNextQuestion);
    connect(backButton, &QPushButton::clicked, this, &EnglishLearner::onBackToWordHome);
    return page;
}

QWidget* EnglishLearner::createPracticeModule(QWidget* parent)
{
    QWidget* page = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->addWidget(createSectionTitle(QStringLiteral("练习中心"), page));
    titleLayout->addStretch();
    QPushButton* statusButton = createActionButton(QStringLiteral("学习状态"), QStringLiteral("ghostButton"), page);
    titleLayout->addWidget(statusButton);
    layout->addLayout(titleLayout);

    QFrame* quizCard = createCard(page, QStringLiteral("entryActionCard"));
    QVBoxLayout* quizLayout = new QVBoxLayout(quizCard);
    quizLayout->setContentsMargins(16, 16, 16, 16);
    quizLayout->setSpacing(10);
    quizLayout->addWidget(createSectionTitle(QStringLiteral("统一题库"), quizCard));
    QLabel* quizText = new QLabel(QStringLiteral("从当前词书生成选择题、拼写题和听音辨义题；答题记录统一进入本地队列。"), quizCard);
    quizText->setObjectName("moduleDescription");
    quizText->setWordWrap(true);
    quizLayout->addWidget(quizText);
    QPushButton* startQuizButton = createActionButton(QStringLiteral("开始专项练习"), QStringLiteral("primaryButton"), quizCard);
    quizLayout->addWidget(startQuizButton);
    layout->addWidget(quizCard);

    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(12);

    QFrame* wrongCard = createCard(page);
    QVBoxLayout* wrongLayout = new QVBoxLayout(wrongCard);
    wrongLayout->setContentsMargins(16, 16, 16, 16);
    wrongLayout->addWidget(createSectionTitle(QStringLiteral("错题复习"), wrongCard));
    QLabel* wrongText = new QLabel(QStringLiteral("按单词、阅读、听力和口语模块筛选错题，答对后可标记已解决。"), wrongCard);
    wrongText->setObjectName("moduleDescription");
    wrongText->setWordWrap(true);
    wrongLayout->addWidget(wrongText);
    wrongLayout->addStretch();

    QFrame* favoriteCard = createCard(page);
    QVBoxLayout* favoriteLayout = new QVBoxLayout(favoriteCard);
    favoriteLayout->setContentsMargins(16, 16, 16, 16);
    favoriteLayout->addWidget(createSectionTitle(QStringLiteral("收藏夹"), favoriteCard));
    QLabel* favoriteText = new QLabel(QStringLiteral("集中查看收藏的单词、题目、文章、听力材料和口语任务。"), favoriteCard);
    favoriteText->setObjectName("moduleDescription");
    favoriteText->setWordWrap(true);
    favoriteLayout->addWidget(favoriteText);
    favoriteLayout->addStretch();

    grid->addWidget(wrongCard, 0, 0);
    grid->addWidget(favoriteCard, 0, 1);
    layout->addLayout(grid, 1);

    connect(statusButton, &QPushButton::clicked, this, &EnglishLearner::onOpenStudyStatus);
    connect(startQuizButton, &QPushButton::clicked, this, &EnglishLearner::onStartReview);
    return page;
}

QWidget* EnglishLearner::createContentModule(QWidget* parent)
{
    QWidget* page = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);

    layout->addWidget(createSectionTitle(QStringLiteral("内容训练"), page));

    QFrame* readingCard = createCard(page, QStringLiteral("entryActionCard"));
    QVBoxLayout* readingLayout = new QVBoxLayout(readingCard);
    readingLayout->setContentsMargins(16, 16, 16, 16);
    readingLayout->setSpacing(10);
    readingLayout->addWidget(createSectionTitle(QStringLiteral("阅读理解"), readingCard));
    QLabel* readingText = new QLabel(QStringLiteral("继续文章阅读、记录阅读位置，并在完成后进入阅读理解题。"), readingCard);
    readingText->setObjectName("moduleDescription");
    readingText->setWordWrap(true);
    readingLayout->addWidget(readingText);
    QPushButton* openReadingButton = createActionButton(QStringLiteral("进入阅读"), QStringLiteral("primaryButton"), readingCard);
    readingLayout->addWidget(openReadingButton);
    layout->addWidget(readingCard);

    QFrame* listeningCard = createCard(page);
    QVBoxLayout* listeningLayout = new QVBoxLayout(listeningCard);
    listeningLayout->setContentsMargins(16, 16, 16, 16);
    listeningLayout->setSpacing(10);
    listeningLayout->addWidget(createSectionTitle(QStringLiteral("听力与口语"), listeningCard));
    QLabel* listeningText = new QLabel(QStringLiteral("完成听力材料、查看文本稿，或进入口语任务进行录音/本地评测。"), listeningCard);
    listeningText->setObjectName("moduleDescription");
    listeningText->setWordWrap(true);
    listeningLayout->addWidget(listeningText);
    QPushButton* openListeningButton = createActionButton(QStringLiteral("进入听说"), QStringLiteral("secondaryButton"), listeningCard);
    listeningLayout->addWidget(openListeningButton);
    layout->addWidget(listeningCard);
    layout->addStretch();

    connect(openReadingButton, &QPushButton::clicked, this, [this]() { switchPage(PageReading); });
    connect(openListeningButton, &QPushButton::clicked, this, [this]() { switchPage(PageListeningSpeaking); });
    return page;
}

QWidget* EnglishLearner::createReadingModule(QWidget* parent)
{
    QWidget* page = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout(page);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);

    QFrame* listCard = createCard(page);
    QVBoxLayout* listLayout = new QVBoxLayout(listCard);
    listLayout->setContentsMargins(16, 16, 16, 16);
    listLayout->addWidget(createSectionTitle(QStringLiteral("阅读材料"), listCard));
    listReading = new QListWidget(listCard);
    listReading->setObjectName("list");
    listLayout->addWidget(listReading, 1);

    QFrame* detailCard = createCard(page);
    QVBoxLayout* detailLayout = new QVBoxLayout(detailCard);
    detailLayout->setContentsMargins(16, 16, 16, 16);
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(createSectionTitle(QStringLiteral("阅读详情"), detailCard));
    topLayout->addStretch();
    QPushButton* statusButton = createActionButton(QStringLiteral("学习状态"), QStringLiteral("ghostButton"), detailCard);
    QPushButton* completeButton = createActionButton(QStringLiteral("完成阅读"), QStringLiteral("primaryButton"), detailCard);
    topLayout->addWidget(statusButton);
    topLayout->addWidget(completeButton);
    detailLayout->addLayout(topLayout);

    labelReadingMeta = new QLabel(detailCard);
    labelReadingMeta->setObjectName("moduleDescription");
    progressReading = new QProgressBar(detailCard);
    progressReading->setRange(0, 100);
    textReadingContent = new QTextBrowser(detailCard);
    textReadingContent->setObjectName("reader");
    detailLayout->addWidget(labelReadingMeta);
    detailLayout->addWidget(progressReading);
    detailLayout->addWidget(textReadingContent, 1);

    layout->addWidget(listCard, 1);
    layout->addWidget(detailCard, 2);

    connect(listReading, &QListWidget::currentItemChanged, this, &EnglishLearner::onReadingSelectionChanged);
    connect(completeButton, &QPushButton::clicked, this, &EnglishLearner::onMarkReadingComplete);
    connect(statusButton, &QPushButton::clicked, this, &EnglishLearner::onOpenStudyStatus);
    return page;
}

QWidget* EnglishLearner::createListeningSpeakingModule(QWidget* parent)
{
    QWidget* page = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout(page);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);

    QFrame* listeningCard = createCard(page);
    QVBoxLayout* listeningLayout = new QVBoxLayout(listeningCard);
    listeningLayout->setContentsMargins(16, 16, 16, 16);
    listeningLayout->addWidget(createSectionTitle(QStringLiteral("听力材料"), listeningCard));
    listListening = new QListWidget(listeningCard);
    listListening->setObjectName("list");
    listeningLayout->addWidget(listListening, 1);
    labelListeningMeta = new QLabel(listeningCard);
    labelListeningMeta->setObjectName("moduleDescription");
    progressListening = new QProgressBar(listeningCard);
    progressListening->setRange(0, 100);
    textListeningTranscript = new QTextBrowser(listeningCard);
    textListeningTranscript->setObjectName("reader");
    QPushButton* finishListening = createActionButton(QStringLiteral("标记完成"), QStringLiteral("primaryButton"), listeningCard);
    listeningLayout->addWidget(labelListeningMeta);
    listeningLayout->addWidget(progressListening);
    listeningLayout->addWidget(textListeningTranscript, 1);
    listeningLayout->addWidget(finishListening);

    QFrame* speakingCard = createCard(page);
    QVBoxLayout* speakingLayout = new QVBoxLayout(speakingCard);
    speakingLayout->setContentsMargins(16, 16, 16, 16);
    speakingLayout->addWidget(createSectionTitle(QStringLiteral("口语任务"), speakingCard));
    listSpeaking = new QListWidget(speakingCard);
    listSpeaking->setObjectName("list");
    textSpeakingPrompt = new QTextBrowser(speakingCard);
    textSpeakingPrompt->setObjectName("reader");
    labelSpeakingMeta = new QLabel(speakingCard);
    labelSpeakingMeta->setObjectName("moduleDescription");
    QPushButton* evaluateButton = createActionButton(QStringLiteral("本地评测一次"), QStringLiteral("primaryButton"), speakingCard);
    speakingLayout->addWidget(listSpeaking, 1);
    speakingLayout->addWidget(labelSpeakingMeta);
    speakingLayout->addWidget(textSpeakingPrompt, 1);
    speakingLayout->addWidget(evaluateButton);

    layout->addWidget(listeningCard, 1);
    layout->addWidget(speakingCard, 1);

    connect(listListening, &QListWidget::currentItemChanged, this, &EnglishLearner::onListeningSelectionChanged);
    connect(finishListening, &QPushButton::clicked, this, &EnglishLearner::onMarkListeningComplete);
    connect(listSpeaking, &QListWidget::currentItemChanged, this, &EnglishLearner::onSpeakingSelectionChanged);
    connect(evaluateButton, &QPushButton::clicked, this, &EnglishLearner::onEvaluateSpeaking);
    return page;
}

QWidget* EnglishLearner::createMineModule(QWidget* parent)
{
    QWidget* page = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout(page);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);

    QFrame* statsCard = createCard(page);
    QVBoxLayout* statsLayout = new QVBoxLayout(statsCard);
    statsLayout->setContentsMargins(16, 16, 16, 16);
    statsLayout->addWidget(createSectionTitle(QStringLiteral("我的学习"), statsCard));
    labelMineStats = new QLabel(statsCard);
    labelMineStats->setObjectName("moduleDescription");
    labelMineStats->setWordWrap(true);
    QPushButton* checkInButton = createActionButton(QStringLiteral("今日打卡"), QStringLiteral("primaryButton"), statsCard);
    QPushButton* shareButton = createActionButton(QStringLiteral("生成分享包"), QStringLiteral("secondaryButton"), statsCard);
    labelLocalStore = new QLabel(statsCard);
    labelLocalStore->setObjectName("moduleDescription");
    labelLocalStore->setWordWrap(true);
    labelOutbox = new QLabel(statsCard);
    labelOutbox->setObjectName("moduleDescription");
    statsLayout->addWidget(labelMineStats);
    statsLayout->addWidget(checkInButton);
    statsLayout->addWidget(shareButton);
    statsLayout->addWidget(labelLocalStore);
    statsLayout->addWidget(labelOutbox);
    statsLayout->addStretch();

    QFrame* communityCard = createCard(page);
    QVBoxLayout* communityLayout = new QVBoxLayout(communityCard);
    communityLayout->setContentsMargins(16, 16, 16, 16);
    QHBoxLayout* communityTop = new QHBoxLayout();
    communityTop->addWidget(createSectionTitle(QStringLiteral("小组与同桌"), communityCard));
    communityTop->addStretch();
    QPushButton* groupButton = createActionButton(QStringLiteral("新建小组"), QStringLiteral("ghostButton"), communityCard);
    communityTop->addWidget(groupButton);
    communityLayout->addLayout(communityTop);
    listCommunity = new QListWidget(communityCard);
    listCommunity->setObjectName("list");
    communityLayout->addWidget(listCommunity, 1);

    QFrame* rankingCard = createCard(page);
    QVBoxLayout* rankingLayout = new QVBoxLayout(rankingCard);
    rankingLayout->setContentsMargins(16, 16, 16, 16);
    QHBoxLayout* rankingTop = new QHBoxLayout();
    rankingTop->addWidget(createSectionTitle(QStringLiteral("排行榜"), rankingCard));
    rankingTop->addStretch();
    QPushButton* refreshRankingButton = createActionButton(QStringLiteral("刷新"), QStringLiteral("ghostButton"), rankingCard);
    rankingTop->addWidget(refreshRankingButton);
    rankingLayout->addLayout(rankingTop);
    listRanking = new QListWidget(rankingCard);
    listRanking->setObjectName("list");
    rankingLayout->addWidget(listRanking, 1);

    layout->addWidget(statsCard, 1);
    layout->addWidget(communityCard, 1);
    layout->addWidget(rankingCard, 1);

    connect(checkInButton, &QPushButton::clicked, this, &EnglishLearner::onCheckIn);
    connect(shareButton, &QPushButton::clicked, this, &EnglishLearner::onCreateSharePackage);
    connect(groupButton, &QPushButton::clicked, this, &EnglishLearner::onCreateStudyGroup);
    connect(refreshRankingButton, &QPushButton::clicked, this, &EnglishLearner::onRefreshRanking);
    return page;
}

QFrame* EnglishLearner::createCard(QWidget* parent, const QString& objectName) const
{
    QFrame* frame = new QFrame(parent);
    frame->setObjectName(objectName.isEmpty() ? QStringLiteral("card") : objectName);
    return frame;
}

QLabel* EnglishLearner::createSectionTitle(const QString& text, QWidget* parent) const
{
    QLabel* label = new QLabel(text, parent);
    label->setObjectName("sectionTitle");
    return label;
}

QPushButton* EnglishLearner::createActionButton(const QString& text, const QString& objectName, QWidget* parent) const
{
    QPushButton* button = new QPushButton(text, parent);
    button->setObjectName(objectName);
    return button;
}

void EnglishLearner::applyTheme()
{
    const QColor windowColor = palette().color(QPalette::Window);
    const bool darkMode = windowColor.lightness() < 128;

    const QString rootStart = darkMode ? "#101827" : "#f4f7ff";
    const QString rootEnd = darkMode ? "#1d2738" : "#edf3ff";
    const QString cardColor = darkMode ? "#1f2a40" : "#ffffff";
    const QString borderColor = darkMode ? "#31415f" : "#d9e0ee";
    const QString textColor = darkMode ? "#e9f0ff" : "#1f2a44";
    const QString mutedColor = darkMode ? "#a8b5cf" : "#60749a";
    const QString accent = darkMode ? "#69b8ff" : "#356ff5";
    const QString soft = darkMode ? "#24324c" : "#f2f6ff";
    const QString hover = darkMode ? "#2e4060" : "#e6edff";
    const QString active = darkMode ? "#345381" : "#dfe8ff";
    const QString warm = darkMode ? "#f4b266" : "#c76b2c";

    setStyleSheet(QString(R"(
QWidget#rootPanel {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2);
}
QFrame#headerCard,
QFrame#card,
QFrame#entryActionCard,
QFrame#sidebarPanel,
QFrame#bottomNavBar,
QFrame#statusPanel {
    background: %3;
    border: 1px solid %4;
    border-radius: 12px;
}
QFrame#todayHeroCard {
    border-radius: 14px;
    border: none;
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %7, stop:1 #42b883);
}
QFrame#todayHeroCard QLabel { color: white; }
QScrollArea#plainScroll {
    border: none;
    background: transparent;
}
QScrollArea#plainScroll > QWidget > QWidget {
    background: transparent;
}
QLabel { color: %5; }
QLabel#appName { font-size: 22px; font-weight: 700; }
QLabel#welcomeText, QLabel#scoreText, QLabel#syncText, QLabel#moduleDescription {
    color: %6;
    font-size: 13px;
}
QLabel#sectionTitle {
    font-size: 17px;
    font-weight: 650;
}
QLabel#navGroupTitle {
    color: %6;
    font-size: 12px;
    font-weight: 700;
    padding: 10px 8px 2px 8px;
}
QLabel#questionWord {
    font-size: 34px;
    font-weight: 750;
}
QLabel#answerFeedback {
    color: %12;
    font-size: 14px;
    font-weight: 600;
}
QPushButton#primaryButton,
QPushButton#secondaryButton,
QPushButton#ghostButton,
QPushButton#navButton,
QPushButton#modeLearnButton,
QPushButton#modeReviewButton,
QPushButton#optionButton {
    border-radius: 10px;
    padding: 8px 13px;
    font-weight: 600;
}
QPushButton#primaryButton,
QPushButton#modeLearnButton {
    background: %7;
    color: white;
    border: none;
}
QPushButton#primaryButton:hover,
QPushButton#modeLearnButton:hover {
    background: #4f87f9;
}
QPushButton#secondaryButton,
QPushButton#ghostButton,
QPushButton#navButton,
QPushButton#modeReviewButton,
QPushButton#optionButton {
    background: %8;
    color: %5;
    border: 1px solid %4;
}
QPushButton#secondaryButton:hover,
QPushButton#ghostButton:hover,
QPushButton#navButton:hover,
QPushButton#modeReviewButton:hover,
QPushButton#optionButton:hover {
    background: %9;
}
QPushButton#navButton {
    text-align: left;
    min-height: 34px;
}
QPushButton#navButton:checked {
    background: %10;
    color: %5;
    border-color: %7;
}
QComboBox, QSpinBox, QListWidget, QTextBrowser {
    background: %8;
    border: 1px solid %4;
    border-radius: 9px;
    color: %5;
    padding: 7px;
}
QListWidget::item {
    padding: 8px;
    border-radius: 7px;
}
QListWidget::item:selected {
    background: %10;
    color: %5;
}
QProgressBar {
    border: 1px solid %4;
    border-radius: 8px;
    text-align: center;
    background: %8;
    min-height: 18px;
    color: %5;
}
QProgressBar::chunk {
    border-radius: 7px;
    background: %7;
}
QTabWidget#navTabs::pane {
    border: 1px solid %4;
    border-radius: 12px;
    background: %3;
}
QTabWidget#navTabs QTabBar::tab {
    min-width: 110px;
    padding: 9px 14px;
    border-top-left-radius: 10px;
    border-top-right-radius: 10px;
    border: 1px solid %4;
    border-bottom: none;
    background: %8;
    color: %6;
    font-weight: 600;
}
QTabWidget#navTabs QTabBar::tab:hover { background: %9; }
QTabWidget#navTabs QTabBar::tab:selected {
    color: %5;
    background: %10;
}
)")
                      .arg(rootStart, rootEnd, cardColor, borderColor, textColor, mutedColor,
                           accent, soft, hover, active, soft, warm));
}

void EnglishLearner::applyAdaptiveLayout()
{
    const bool shouldCompact = width() < 760 || height() > width() * 1.25;
    compactMode = shouldCompact;
    if (sidebarPanel != nullptr) {
        sidebarPanel->setVisible(!compactMode);
    }
    if (statusPanel != nullptr) {
        statusPanel->setVisible(!compactMode);
    }
    if (bottomNavBar != nullptr) {
        bottomNavBar->setVisible(compactMode);
    }
    if (labelWelcome != nullptr) {
        labelWelcome->setVisible(!compactMode);
    }
    if (labelScore != nullptr) {
        labelScore->setVisible(!compactMode);
    }

    if (compactMode) {
        setMinimumSize(390, 620);
    } else {
        setMinimumSize(980, 660);
    }
    updateNavigationState();
}

void EnglishLearner::switchPage(int pageIndex)
{
    if (pageStack == nullptr || pageIndex < 0 || pageIndex >= pageStack->count()) {
        return;
    }
    currentPageIndex = pageIndex;
    pageStack->setCurrentIndex(pageIndex);
    updateNavigationState();
}

void EnglishLearner::updateNavigationState()
{
    const auto updateButtons = [this](const QList<QPushButton*>& buttons) {
        for (QPushButton* button : buttons) {
            if (button == nullptr) {
                continue;
            }
            const int buttonPage = button->property("pageIndex").toInt();
            const bool selected = buttonPage == currentPageIndex
                || (buttonPage == PageContent
                    && (currentPageIndex == PageReading || currentPageIndex == PageListeningSpeaking));
            button->setChecked(selected);
        }
    };
    updateButtons(sideNavButtons);
    updateButtons(bottomNavButtons);
}

void EnglishLearner::updateStatusPanel()
{
    if (labelPanelToday != nullptr) {
        labelPanelToday->setText(todaySummaryText());
    }

    if (labelPanelPlan != nullptr) {
        const QList<LocalBook> books = m_store.books();
        if (books.isEmpty()) {
            labelPanelPlan->setText(QStringLiteral("暂无词书，请导入本地内容或等待服务端下发。"));
        } else {
            LocalBook selected = books.first();
            const qint64 selectedId = comboBooks == nullptr ? 0 : comboBooks->currentData().toLongLong();
            for (const LocalBook& book : books) {
                if (book.localId == selectedId) {
                    selected = book;
                    break;
                }
            }
            const int percent = selected.itemCount <= 0 ? 0 : qMin(100, selected.learnedCount * 100 / selected.itemCount);
            labelPanelPlan->setText(QStringLiteral("%1\n进度 %2%，待复习 %3，已掌握 %4。")
                                        .arg(selected.title)
                                        .arg(percent)
                                        .arg(selected.dueCount)
                                        .arg(selected.masteredCount));
        }
    }

    if (labelPanelSync != nullptr) {
        labelPanelSync->setText(QStringLiteral("%1\n待同步变更 %2，未同步学习事件 %3。")
                                    .arg(syncStatusText())
                                    .arg(m_store.pendingOutboxCount())
                                    .arg(m_store.unsyncedEventCount()));
    }
}

void EnglishLearner::updateDailyVisual()
{
    const QDate today = QDate::currentDate();
    if (labelTodayDate != nullptr) {
        labelTodayDate->setText(QStringLiteral("%1 · 今日学习").arg(today.toString("yyyy-MM-dd")));
    }
    if (labelTodayFocus != nullptr) {
        labelTodayFocus->setText(QStringLiteral("今日励志词：%1").arg(generateDailyWord()));
    }
    if (labelTodayTaskSummary != nullptr) {
        labelTodayTaskSummary->setText(generateDailyMeaning());
    }
    labelDate->setText(QStringLiteral("今日主题 - %1").arg(today.toString("yyyy-MM-dd")));
    labelDailyWord->setText(QStringLiteral("今日励志词：%1").arg(generateDailyWord()));
    labelDailyMeaning->setText(generateDailyMeaning());

    const QStringList startColors{
        "#4c7bf3", "#4e9fcf", "#5b8c6f", "#7a6bd6", "#b07053", "#3979a8", "#5b7ac2"
    };
    const QStringList endColors{
        "#65b2ff", "#71c2d8", "#7bc285", "#8f87ec", "#d08a66", "#58a2d6", "#7b9ce5"
    };
    const int idx = today.dayOfYear() % startColors.size();
    heroFrame->setStyleSheet(QString(R"(
QFrame#heroFrame {
    border-radius: 14px;
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2);
}
QFrame#heroFrame QLabel { color: white; }
QLabel#heroDate { color: rgba(255,255,255,0.86); font-size: 13px; }
QLabel#dailyWord { font-size: 26px; font-weight: 750; }
QLabel#dailyMeaning { color: rgba(255,255,255,0.95); font-size: 14px; }
)")
                                 .arg(startColors[idx], endColors[idx]));
}

void EnglishLearner::onLoginSuccess(const UserProfile& user)
{
    m_currentUser = user;
    QString storeError;
    if (!m_store.openForUser(m_currentUser, &storeError)) {
        QMessageBox::warning(this,
                             QStringLiteral("本地库初始化失败"),
                             QStringLiteral("客户端仍可使用界面，但离线数据无法保存：%1").arg(storeError));
    }

    setWindowTitle(QStringLiteral("LingoPulse 客户端 - %1").arg(toDisplayName(m_currentUser.username)));
    labelWelcome->setText(QStringLiteral("欢迎，%1").arg(toDisplayName(m_currentUser.username)));
    refreshAll();
    switchPage(PageToday);
    onLoadNextQuestion();
    onRefreshRanking();
}

void EnglishLearner::refreshAll()
{
    updateScoreDisplay();
    refreshBooks();
    refreshTodaySummary();
    refreshReadingList();
    refreshListeningList();
    refreshSpeakingList();
    refreshCommunityList();
    refreshSyncState();
}

void EnglishLearner::refreshBooks()
{
    comboBooks->clear();
    const QList<LocalBook> books = m_store.books();
    for (const LocalBook& book : books) {
        comboBooks->addItem(QStringLiteral("%1 · %2 · %3词")
                                .arg(book.title, book.level)
                                .arg(book.itemCount),
                            book.localId);
    }
    updateWordProgressDisplay();
}

void EnglishLearner::refreshTodaySummary()
{
    const LocalStudySummary summary = m_store.todaySummary();
    const int totalAnswered = summary.correctCount + summary.wrongCount;
    const int accuracy = totalAnswered == 0 ? 0 : (summary.correctCount * 100 / totalAnswered);
    const int completion = qMin(100, summary.completedTasks * 20);
    if (progressTodayCompletion != nullptr) {
        progressTodayCompletion->setValue(completion);
    }
    if (labelTodayStatus != nullptr) {
        labelTodayStatus->setText(todaySummaryText());
    }
    labelTodayStats->setText(QStringLiteral(
                                 "新学 %1 个，复习 %2 个，正确率 %3%，学习 %4 分钟，今日积分 %+5，完成任务 %6 个。")
                                 .arg(summary.newWords)
                                 .arg(summary.reviewWords)
                                 .arg(accuracy)
                                 .arg(summary.studySeconds / 60)
                                 .arg(summary.scoreDelta)
                                 .arg(summary.completedTasks));
    labelMineStats->setText(QStringLiteral(
                                "总积分 %1，连续学习天数 %2。今日新学 %3，复习 %4，正确 %5，错误 %6。")
                                .arg(m_currentUser.totalScore)
                                .arg(qMax(1, m_currentUser.studyDays))
                                .arg(summary.newWords)
                                .arg(summary.reviewWords)
                                .arg(summary.correctCount)
                                .arg(summary.wrongCount));
    updateStatusPanel();
}

void EnglishLearner::refreshReadingList()
{
    listReading->clear();
    const QList<LocalReadingItem> items = m_store.readingItems();
    for (const LocalReadingItem& item : items) {
        QListWidgetItem* row = new QListWidgetItem(
            QStringLiteral("%1\n%2 · %3词 · %4%")
                .arg(item.title, item.level)
                .arg(item.wordCount)
                .arg(item.progressPercent),
            listReading);
        row->setData(Qt::UserRole, item.localId);
    }
    if (listReading->count() > 0) {
        listReading->setCurrentRow(0);
    }
}

void EnglishLearner::refreshListeningList()
{
    listListening->clear();
    const QList<LocalListeningItem> items = m_store.listeningItems();
    for (const LocalListeningItem& item : items) {
        const int percent = item.durationSeconds <= 0 ? 0 : item.listenSeconds * 100 / item.durationSeconds;
        QListWidgetItem* row = new QListWidgetItem(
            QStringLiteral("%1\n%2 · %3秒 · %4%")
                .arg(item.title, item.level)
                .arg(item.durationSeconds)
                .arg(percent),
            listListening);
        row->setData(Qt::UserRole, item.localId);
    }
    if (listListening->count() > 0) {
        listListening->setCurrentRow(0);
    }
}

void EnglishLearner::refreshSpeakingList()
{
    listSpeaking->clear();
    const QList<LocalSpeakingTask> tasks = m_store.speakingTasks();
    for (const LocalSpeakingTask& task : tasks) {
        QListWidgetItem* row = new QListWidgetItem(
            QStringLiteral("%1\n%2 · 上次分数 %3")
                .arg(task.title, task.level)
                .arg(task.lastScore),
            listSpeaking);
        row->setData(Qt::UserRole, task.localId);
    }
    if (listSpeaking->count() > 0) {
        listSpeaking->setCurrentRow(0);
    }
}

void EnglishLearner::refreshCommunityList()
{
    listCommunity->clear();
    const QList<LocalCommunityItem> items = m_store.communityItems();
    for (const LocalCommunityItem& item : items) {
        new QListWidgetItem(QStringLiteral("%1\n%2 · %3").arg(item.name, item.kind, item.status), listCommunity);
    }
}

void EnglishLearner::refreshSyncState()
{
    labelSyncStatus->setText(syncStatusText());
    labelLocalStore->setText(QStringLiteral("本地 SQLite：%1\n设备 ID：%2")
                                 .arg(m_store.databasePath(), m_store.deviceUuid()));
    labelOutbox->setText(QStringLiteral("待同步变更：%1，未同步学习事件：%2")
                             .arg(m_store.pendingOutboxCount())
                             .arg(m_store.unsyncedEventCount()));
    updateStatusPanel();
}

void EnglishLearner::updateScoreDisplay()
{
    labelScore->setText(QStringLiteral("积分：%1  学习天数：%2")
                            .arg(m_currentUser.totalScore)
                            .arg(qMax(1, m_currentUser.studyDays)));
}

void EnglishLearner::updateWordProgressDisplay()
{
    const QList<LocalBook> books = m_store.books();
    if (books.isEmpty()) {
        labelWordProgress->setText(QStringLiteral("暂无词书，请导入本地内容或等待服务端下发。"));
        progressDaily->setValue(0);
        return;
    }

    LocalBook selected = books.first();
    const qint64 selectedId = comboBooks->currentData().toLongLong();
    for (const LocalBook& book : books) {
        if (book.localId == selectedId) {
            selected = book;
            break;
        }
    }

    const int percent = selected.itemCount <= 0 ? 0 : qMin(100, selected.learnedCount * 100 / selected.itemCount);
    progressDaily->setValue(percent);
    labelWordProgress->setText(QStringLiteral(
                                   "%1：总词数 %2，已学 %3，待复习 %4，已掌握 %5。存储模式：%6，来源：%7。")
                                   .arg(selected.title)
                                   .arg(selected.itemCount)
                                   .arg(selected.learnedCount)
                                   .arg(selected.dueCount)
                                   .arg(selected.masteredCount)
                                   .arg(selected.storageMode, selected.sourceType));
    updateStatusPanel();
}

void EnglishLearner::onNavigate()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button == nullptr) {
        return;
    }
    switchPage(button->property("pageIndex").toInt());
}

void EnglishLearner::onOpenStudyStatus()
{
    const LocalStudySummary summary = m_store.todaySummary();
    updateStatusPanel();
    if (!compactMode && statusPanel != nullptr) {
        statusPanel->setVisible(true);
        return;
    }

    QMessageBox::information(this,
                             QStringLiteral("学习状态"),
                             QStringLiteral("今日新学：%1\n今日复习：%2\n正确：%3\n错误：%4\n待同步：%5")
                                 .arg(summary.newWords)
                                 .arg(summary.reviewWords)
                                 .arg(summary.correctCount)
                                 .arg(summary.wrongCount)
                                 .arg(m_store.pendingOutboxCount()));
}

void EnglishLearner::onStartLearn()
{
    switchPage(PageWords);
    enterPracticeMode(QStringLiteral("LEARN"));
    wordStack->setCurrentWidget(wordLearnPage);
}

void EnglishLearner::onStartReview()
{
    switchPage(PageWords);
    enterPracticeMode(QStringLiteral("REVIEW"));
    wordStack->setCurrentWidget(wordLearnPage);
}

void EnglishLearner::onBackToWordHome()
{
    switchPage(PageWords);
    wordStack->setCurrentWidget(wordHomePage);
    refreshAll();
}

void EnglishLearner::onSavePlan()
{
    const qint64 bookId = comboBooks->currentData().toLongLong();
    if (bookId <= 0) {
        QMessageBox::warning(this, QStringLiteral("计划设置"), QStringLiteral("请先选择词书。"));
        return;
    }

    if (m_store.savePlan(bookId, spinDailyNew->value(), spinDailyReview->value())) {
        QMessageBox::information(this, QStringLiteral("计划设置"), QStringLiteral("计划已保存，并写入待同步队列。"));
        refreshSyncState();
    } else {
        QMessageBox::warning(this, QStringLiteral("计划设置"), QStringLiteral("计划保存失败。"));
    }
}

void EnglishLearner::enterPracticeMode(const QString& activityType)
{
    m_currentActivityType = activityType;
    labelAnswerFeedback->clear();
    onLoadNextQuestion();
}

void EnglishLearner::onLoadNextQuestion()
{
    QuizQuestion question;
    QString errorMessage;
    const bool fromServer = !m_apiClient.token().isEmpty()
        && m_apiClient.fetchNextQuestion(&question, &errorMessage);
    if (!fromServer) {
        question = fallbackQuestion();
    }
    setCurrentQuestion(question, fromServer);
}

void EnglishLearner::setCurrentQuestion(const QuizQuestion& question, bool fromServer)
{
    m_currentQuestion = question;
    m_currentQuestionFromServer = fromServer;
    labelQuestionWord->setText(question.word);
    labelQuestionMeta->setText(QStringLiteral("%1%2")
                                   .arg(question.phonetic.isEmpty() ? QStringLiteral("选择最合适的释义。") : question.phonetic)
                                   .arg(fromServer ? QStringLiteral(" · 来自服务端题库") : QStringLiteral(" · 本地离线题")));
    labelQuestionExample->setText(question.exampleSentence.isEmpty()
                                      ? QStringLiteral("答题后会记录到本地学习事件，联网后进入同步队列。")
                                      : question.exampleSentence);
    labelAnswerFeedback->clear();

    for (int i = 0; i < optionButtons.size(); ++i) {
        QPushButton* button = optionButtons[i];
        if (i < question.options.size()) {
            button->setText(question.options[i]);
            button->setEnabled(true);
            button->show();
        } else {
            button->setEnabled(false);
            button->hide();
        }
    }
}

QuizQuestion EnglishLearner::fallbackQuestion() const
{
    static const QList<QuizQuestion> questions{
        {1001, QStringLiteral("persist"), QStringLiteral("/pərˈsɪst/"),
         QStringLiteral("坚持"), QStringLiteral("She will persist until the task is done."),
         QStringLiteral("坚持"), {QStringLiteral("坚持"), QStringLiteral("逃避"), QStringLiteral("装饰"), QStringLiteral("推迟")}},
        {1002, QStringLiteral("clarity"), QStringLiteral("/ˈklærəti/"),
         QStringLiteral("清晰"), QStringLiteral("Good notes bring clarity to review."),
         QStringLiteral("清晰"), {QStringLiteral("清晰"), QStringLiteral("争吵"), QStringLiteral("疲惫"), QStringLiteral("潮湿")}},
        {1003, QStringLiteral("momentum"), QStringLiteral("/moʊˈmentəm/"),
         QStringLiteral("势头"), QStringLiteral("Small wins create learning momentum."),
         QStringLiteral("势头"), {QStringLiteral("势头"), QStringLiteral("限制"), QStringLiteral("口音"), QStringLiteral("线索")}},
        {1004, QStringLiteral("evaluate"), QStringLiteral("/ɪˈvæljueɪt/"),
         QStringLiteral("评估"), QStringLiteral("The app can evaluate a speaking attempt locally."),
         QStringLiteral("评估"), {QStringLiteral("评估"), QStringLiteral("隐藏"), QStringLiteral("借出"), QStringLiteral("擦除")}}
    };

    const int index = static_cast<int>(QRandomGenerator::global()->bounded(questions.size()));
    return questions[index];
}

void EnglishLearner::onAnswerOption()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button == nullptr) {
        return;
    }

    const QString selected = button->text();
    SubmitAnswerResult result;
    QString errorMessage;
    bool ok = false;
    if (m_currentQuestionFromServer && !m_apiClient.token().isEmpty()) {
        ok = m_apiClient.submitAnswer(m_currentQuestion.wordId, selected, &result, &errorMessage);
    }

    if (!ok) {
        result.correct = !m_currentQuestion.correctAnswer.isEmpty()
            && selected == m_currentQuestion.correctAnswer;
        result.correctAnswer = m_currentQuestion.correctAnswer.isEmpty()
            ? m_currentQuestion.translation
            : m_currentQuestion.correctAnswer;
        result.scoreDelta = result.correct ? 2 : 0;
        result.totalScore = m_currentUser.totalScore + result.scoreDelta;
    }

    m_currentUser.totalScore = result.totalScore;
    m_store.recordWordStudy(m_currentUser.userId,
                            m_currentQuestion,
                            selected,
                            result.correct,
                            result.scoreDelta,
                            m_currentActivityType);
    m_store.saveUserProfile(m_currentUser);

    labelAnswerFeedback->setText(result.correct
                                     ? QStringLiteral("回答正确，积分 +%1。").arg(result.scoreDelta)
                                     : QStringLiteral("回答错误。正确答案：%1").arg(result.correctAnswer));
    updateScoreDisplay();
    refreshTodaySummary();
    refreshSyncState();
    updateWordProgressDisplay();
}

void EnglishLearner::onReadingSelectionChanged()
{
    QListWidgetItem* current = listReading->currentItem();
    if (current == nullptr) {
        return;
    }
    const qint64 id = current->data(Qt::UserRole).toLongLong();
    const QList<LocalReadingItem> items = m_store.readingItems();
    for (const LocalReadingItem& item : items) {
        if (item.localId == id) {
            labelReadingMeta->setText(QStringLiteral("%1 · %2词 · 预计 %3 分钟 · 来源 %4")
                                          .arg(item.level)
                                          .arg(item.wordCount)
                                          .arg(item.estimatedMinutes)
                                          .arg(item.sourceType));
            progressReading->setValue(item.progressPercent);
            textReadingContent->setText(item.content);
            break;
        }
    }
}

void EnglishLearner::onMarkReadingComplete()
{
    QListWidgetItem* current = listReading->currentItem();
    if (current == nullptr) {
        return;
    }
    if (m_store.completeReading(current->data(Qt::UserRole).toLongLong())) {
        refreshReadingList();
        refreshTodaySummary();
        refreshSyncState();
    }
}

void EnglishLearner::onListeningSelectionChanged()
{
    QListWidgetItem* current = listListening->currentItem();
    if (current == nullptr) {
        return;
    }
    const qint64 id = current->data(Qt::UserRole).toLongLong();
    const QList<LocalListeningItem> items = m_store.listeningItems();
    for (const LocalListeningItem& item : items) {
        if (item.localId == id) {
            const int percent = item.durationSeconds <= 0 ? 0 : item.listenSeconds * 100 / item.durationSeconds;
            labelListeningMeta->setText(QStringLiteral("%1 · %2秒 · 已完成 %3 次")
                                            .arg(item.level)
                                            .arg(item.durationSeconds)
                                            .arg(item.completedCount));
            progressListening->setValue(percent);
            textListeningTranscript->setText(item.transcript);
            break;
        }
    }
}

void EnglishLearner::onMarkListeningComplete()
{
    QListWidgetItem* current = listListening->currentItem();
    if (current == nullptr) {
        return;
    }
    if (m_store.completeListening(current->data(Qt::UserRole).toLongLong())) {
        refreshListeningList();
        refreshTodaySummary();
        refreshSyncState();
    }
}

void EnglishLearner::onSpeakingSelectionChanged()
{
    QListWidgetItem* current = listSpeaking->currentItem();
    if (current == nullptr) {
        return;
    }
    const qint64 id = current->data(Qt::UserRole).toLongLong();
    const QList<LocalSpeakingTask> tasks = m_store.speakingTasks();
    for (const LocalSpeakingTask& task : tasks) {
        if (task.localId == id) {
            labelSpeakingMeta->setText(QStringLiteral("%1 · 评测模式 %2 · 上次分数 %3")
                                           .arg(task.level,
                                                task.lastMode.isEmpty() ? QStringLiteral("未评测") : task.lastMode)
                                           .arg(task.lastScore));
            textSpeakingPrompt->setText(QStringLiteral("%1\n\n参考表达：\n%2")
                                            .arg(task.prompt, task.referenceText));
            break;
        }
    }
}

void EnglishLearner::onEvaluateSpeaking()
{
    QListWidgetItem* current = listSpeaking->currentItem();
    if (current == nullptr) {
        return;
    }
    const int score = 65 + static_cast<int>(QRandomGenerator::global()->bounded(31));
    if (m_store.saveSpeakingAttempt(current->data(Qt::UserRole).toLongLong(), score, QStringLiteral("LOCAL"))) {
        refreshSpeakingList();
        refreshTodaySummary();
        refreshSyncState();
    }
}

void EnglishLearner::onCheckIn()
{
    if (m_store.checkIn(m_currentUser.userId, 5)) {
        m_currentUser.totalScore += 5;
        m_store.saveUserProfile(m_currentUser);
        QMessageBox::information(this, QStringLiteral("打卡成功"), QStringLiteral("今日打卡完成，积分 +5。"));
        refreshAll();
    }
}

void EnglishLearner::onRefreshRanking()
{
    listRanking->clear();
    QList<RankingItem> rankings;
    QString errorMessage;
    if (m_apiClient.fetchRanking(10, &rankings, &errorMessage)) {
        for (const RankingItem& item : rankings) {
            new QListWidgetItem(QStringLiteral("#%1  %2  %3分")
                                    .arg(item.rank)
                                    .arg(toDisplayName(item.username))
                                    .arg(item.totalScore),
                                listRanking);
        }
        return;
    }

    new QListWidgetItem(QStringLiteral("#1  %1  %2分")
                            .arg(toDisplayName(m_currentUser.username))
                            .arg(m_currentUser.totalScore),
                        listRanking);
    new QListWidgetItem(QStringLiteral("#2  Alex  96分"), listRanking);
    new QListWidgetItem(QStringLiteral("#3  Mira  88分"), listRanking);
}

void EnglishLearner::onSyncNow()
{
    if (m_store.markAllSynced()) {
        QMessageBox::information(this, QStringLiteral("同步"), QStringLiteral("已模拟完成同步，待同步队列已清空。"));
    }
    refreshSyncState();
}

void EnglishLearner::onCreateSharePackage()
{
    const QString target = QInputDialog::getText(this,
                                                QStringLiteral("分享传输"),
                                                QStringLiteral("输入好友、同桌或小组名称："),
                                                QLineEdit::Normal,
                                                QStringLiteral("同桌：Alex"));
    if (target.isEmpty()) {
        return;
    }
    if (m_store.createShareRecord(QStringLiteral("FRIEND"), target)) {
        QMessageBox::information(this, QStringLiteral("分享传输"), QStringLiteral("分享授权已生成，内容正文仍保留在本地。"));
        refreshSyncState();
    }
}

void EnglishLearner::onImportLocalContent()
{
    QMessageBox::information(this,
                             QStringLiteral("本地导入"),
                             QStringLiteral("本地导入入口已就绪。当前版本使用种子本地材料演示；后续可接入文件解析并写入 local_book_content。"));
}

void EnglishLearner::onCreateStudyGroup()
{
    const QString name = QInputDialog::getText(this,
                                              QStringLiteral("新建学习小组"),
                                              QStringLiteral("小组名称："),
                                              QLineEdit::Normal,
                                              QStringLiteral("晚间复习小组"));
    if (name.isEmpty()) {
        return;
    }
    if (m_store.createStudyGroup(name)) {
        refreshCommunityList();
        refreshSyncState();
    }
}

QString EnglishLearner::todaySummaryText() const
{
    const LocalStudySummary summary = m_store.todaySummary();
    const int totalAnswered = summary.correctCount + summary.wrongCount;
    const int accuracy = totalAnswered == 0 ? 0 : summary.correctCount * 100 / totalAnswered;
    return QStringLiteral("新学 %1，复习 %2，正确率 %3%，学习 %4 分钟，积分 %+5，完成任务 %6。")
        .arg(summary.newWords)
        .arg(summary.reviewWords)
        .arg(accuracy)
        .arg(summary.studySeconds / 60)
        .arg(summary.scoreDelta)
        .arg(summary.completedTasks);
}

QString EnglishLearner::generateDailyWord() const
{
    static const QStringList words{
        "Persist", "Focus", "Brave", "Explore", "Growth", "Clarity", "Momentum",
        "Discipline", "Vision", "Spark", "Steady", "Refine", "Courage", "Thrive"
    };
    return words[QDate::currentDate().dayOfYear() % words.size()];
}

QString EnglishLearner::generateDailyMeaning() const
{
    static const QStringList meanings{
        QStringLiteral("小步坚持，长期一定有大变化。"),
        QStringLiteral("稳定节奏，比短期爆发更有效。"),
        QStringLiteral("每天复盘一点点，学习效果会明显提升。"),
        QStringLiteral("敢开口，才能把听说能力真正练起来。"),
        QStringLiteral("先完成再优化，每一次练习都算数。"),
        QStringLiteral("保持好奇心，你会更快进入英语语境。"),
        QStringLiteral("输入和输出双线并行，英语能力成长更快。")
    };
    return meanings[QDate::currentDate().dayOfYear() % meanings.size()];
}

QString EnglishLearner::toDisplayName(const QString& rawUsername) const
{
    if (!m_currentUser.displayName.isEmpty() && rawUsername == m_currentUser.username) {
        return m_currentUser.displayName;
    }

    const int sep = rawUsername.indexOf(':');
    if (sep <= 0) {
        return rawUsername;
    }

    const QString type = rawUsername.left(sep).toLower();
    const QString account = rawUsername.mid(sep + 1);
    if (account.isEmpty()) {
        return rawUsername;
    }
    if (type == QStringLiteral("phone")) {
        return QStringLiteral("手机号(%1)").arg(account);
    }
    if (type == QStringLiteral("wechat")) {
        return QStringLiteral("微信(%1)").arg(account);
    }
    if (type == QStringLiteral("qq")) {
        return QStringLiteral("QQ(%1)").arg(account);
    }
    if (type == QStringLiteral("email")) {
        return QStringLiteral("邮箱(%1)").arg(account);
    }
    if (type == QStringLiteral("offline")) {
        return QStringLiteral("离线学习者");
    }
    return account;
}

QString EnglishLearner::syncStatusText() const
{
    if (!m_store.isOpen()) {
        return QStringLiteral("本地库未打开");
    }
    const int pending = m_store.pendingOutboxCount();
    if (pending == 0) {
        return QStringLiteral("已同步");
    }
    return QStringLiteral("待同步 %1").arg(pending);
}
