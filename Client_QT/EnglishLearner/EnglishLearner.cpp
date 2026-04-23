#include "EnglishLearner.h"

#include "LoginDialog.h"

#include <QAbstractItemView>
#include <QColor>
#include <QDate>
#include <QDialog>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPalette>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QtGlobal>

EnglishLearner::EnglishLearner(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("LingoPulse \u5ba2\u6237\u7aef"));
    resize(980, 720);
    setMinimumSize(860, 620);

    QWidget* rootPanel = new QWidget(this);
    rootPanel->setObjectName("rootPanel");
    setCentralWidget(rootPanel);

    QVBoxLayout* rootLayout = new QVBoxLayout(rootPanel);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(14);

    QFrame* headerCard = new QFrame(rootPanel);
    headerCard->setObjectName("headerCard");
    QHBoxLayout* headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setContentsMargins(16, 12, 16, 12);
    headerLayout->setSpacing(12);

    labelAppName = new QLabel(QStringLiteral("LingoPulse"), headerCard);
    labelAppName->setObjectName("appName");

    labelWelcome = new QLabel(QStringLiteral("\u8bbf\u5ba2"), headerCard);
    labelWelcome->setObjectName("welcomeText");
    labelScore = new QLabel(QStringLiteral("\u79ef\u5206\uff1a0"), headerCard);
    labelScore->setObjectName("scoreText");

    headerLayout->addWidget(labelAppName);
    headerLayout->addStretch();
    headerLayout->addWidget(labelWelcome);
    headerLayout->addWidget(labelScore);

    rootLayout->addWidget(headerCard);

    navTabs = new QTabWidget(rootPanel);
    navTabs->setObjectName("navTabs");
    navTabs->setDocumentMode(true);
    navTabs->setTabPosition(QTabWidget::South);
    rootLayout->addWidget(navTabs, 1);

    QWidget* wordModule = new QWidget(navTabs);
    QVBoxLayout* wordLayout = new QVBoxLayout(wordModule);
    wordLayout->setContentsMargins(14, 14, 14, 14);
    wordLayout->setSpacing(12);

    QHBoxLayout* moduleTitleLayout = new QHBoxLayout();
    QLabel* labelModuleTitle = new QLabel(QStringLiteral("\u8bcd\u6c47"), wordModule);
    labelModuleTitle->setObjectName("moduleTitle");
    btnStudyStatus = new QPushButton(QStringLiteral("\u5b66\u4e60\u72b6\u6001"), wordModule);
    btnStudyStatus->setObjectName("ghostButton");
    moduleTitleLayout->addWidget(labelModuleTitle);
    moduleTitleLayout->addStretch();
    moduleTitleLayout->addWidget(btnStudyStatus);
    wordLayout->addLayout(moduleTitleLayout);

    heroFrame = new QFrame(wordModule);
    heroFrame->setObjectName("heroFrame");
    QVBoxLayout* heroLayout = new QVBoxLayout(heroFrame);
    heroLayout->setContentsMargins(16, 14, 16, 14);
    heroLayout->setSpacing(10);

    QHBoxLayout* heroTopLayout = new QHBoxLayout();
    labelDate = new QLabel(heroFrame);
    labelDate->setObjectName("heroDate");
    btnWordBook = new QPushButton(QStringLiteral("\u4eca\u65e5\u8bcd\u4e66\uff1a\u6838\u5fc3\u8bcd\u6c47"), heroFrame);
    btnWordBook->setObjectName("pillButton");
    heroTopLayout->addWidget(labelDate);
    heroTopLayout->addStretch();
    heroTopLayout->addWidget(btnWordBook);
    heroLayout->addLayout(heroTopLayout);

    labelDailyWord = new QLabel(heroFrame);
    labelDailyWord->setObjectName("dailyWord");
    labelDailyMeaning = new QLabel(heroFrame);
    labelDailyMeaning->setObjectName("dailyMeaning");
    labelDailyMeaning->setWordWrap(true);

    heroLayout->addWidget(labelDailyWord);
    heroLayout->addWidget(labelDailyMeaning);
    wordLayout->addWidget(heroFrame);

    QFrame* quizCard = new QFrame(wordModule);
    quizCard->setObjectName("card");
    QVBoxLayout* quizLayout = new QVBoxLayout(quizCard);
    quizLayout->setContentsMargins(16, 16, 16, 16);
    quizLayout->setSpacing(12);

    labelWord = new QLabel(QStringLiteral("\u8bf7\u5148\u767b\u5f55"), quizCard);
    labelWord->setObjectName("wordLabel");
    labelWord->setAlignment(Qt::AlignCenter);
    labelWord->setMinimumHeight(84);
    quizLayout->addWidget(labelWord);

    for (int i = 0; i < 4; ++i) {
        radioButtons[i] = new QRadioButton(quizCard);
        radioButtons[i]->setObjectName("optionButton");
        radioButtons[i]->setEnabled(false);
        quizLayout->addWidget(radioButtons[i]);
    }

    QHBoxLayout* actionLayout = new QHBoxLayout();
    btnSubmit = new QPushButton(QStringLiteral("\u63d0\u4ea4\u7b54\u6848"), quizCard);
    btnSubmit->setObjectName("primaryButton");
    btnSubmit->setEnabled(false);
    btnRanking = new QPushButton(QStringLiteral("\u6392\u884c\u699c"), quizCard);
    btnRanking->setObjectName("secondaryButton");
    actionLayout->addWidget(btnSubmit);
    actionLayout->addWidget(btnRanking);
    quizLayout->addLayout(actionLayout);

    labelFeedback = new QLabel(quizCard);
    labelFeedback->setObjectName("feedbackText");
    labelFeedback->setAlignment(Qt::AlignCenter);
    labelFeedback->setWordWrap(true);
    quizLayout->addWidget(labelFeedback);

    wordLayout->addWidget(quizCard);

    QFrame* progressCard = new QFrame(wordModule);
    progressCard->setObjectName("card");
    QVBoxLayout* progressLayout = new QVBoxLayout(progressCard);
    progressLayout->setContentsMargins(16, 12, 16, 12);
    progressLayout->setSpacing(8);

    QLabel* progressTitle = new QLabel(QStringLiteral("\u6bcf\u65e5\u8fdb\u5ea6"), progressCard);
    progressTitle->setObjectName("sectionTitle");
    progressLayout->addWidget(progressTitle);

    progressDaily = new QProgressBar(progressCard);
    progressDaily->setRange(0, m_dailyGoal);
    progressDaily->setValue(0);
    progressDaily->setFormat("%v / %m");
    progressLayout->addWidget(progressDaily);

    labelProgress = new QLabel(progressCard);
    labelProgress->setObjectName("progressText");
    labelGoal = new QLabel(progressCard);
    labelGoal->setObjectName("progressHint");
    labelGoal->setWordWrap(true);
    progressLayout->addWidget(labelProgress);
    progressLayout->addWidget(labelGoal);
    wordLayout->addWidget(progressCard);

    navTabs->addTab(wordModule, QStringLiteral("\u8bcd\u6c47"));
    navTabs->addTab(createPlaceholderModule(
                        QStringLiteral("\u542c\u8bf4\u8bad\u7ec3"),
                        QStringLiteral("\u8be5\u6a21\u5757\u5df2\u9884\u7559\uff0c\u53ef\u63a5\u5165\u542c\u529b\u8bad\u7ec3\u3001\u8ddf\u8bfb\u6253\u5206\u548c\u53e3\u8bed\u8bc4\u6d4b\u3002")),
                    QStringLiteral("\u542c\u8bf4"));
    navTabs->addTab(createPlaceholderModule(
                        QStringLiteral("\u9605\u8bfb\u7406\u89e3"),
                        QStringLiteral("\u8be5\u6a21\u5757\u5df2\u9884\u7559\uff0c\u53ef\u63a5\u5165\u5206\u7ea7\u9605\u8bfb\u3001\u9898\u76ee\u8bb2\u89e3\u548c\u9519\u9898\u590d\u76d8\u3002")),
                    QStringLiteral("\u9605\u8bfb"));

    connect(btnSubmit, &QPushButton::clicked, this, &EnglishLearner::onSubmit);
    connect(btnRanking, &QPushButton::clicked, this, &EnglishLearner::onShowRanking);
    connect(btnWordBook, &QPushButton::clicked, this, &EnglishLearner::onOpenWordBook);
    connect(btnStudyStatus, &QPushButton::clicked, this, &EnglishLearner::onOpenStudyStatus);

    applyTheme();
    updateDailyVisual();
    updateDailyProgress(0);

    LoginDialog loginDlg(&m_apiClient, this);
    if (loginDlg.exec() != QDialog::Accepted) {
        QTimer::singleShot(0, this, &QWidget::close);
        return;
    }

    onLoginSuccess(loginDlg.loggedInUser());
}

EnglishLearner::~EnglishLearner() = default;

void EnglishLearner::applyTheme()
{
    const QColor windowColor = palette().color(QPalette::Window);
    const bool darkMode = windowColor.lightness() < 128;

    const QString rootStart = darkMode ? "#111829" : "#f4f7ff";
    const QString rootEnd = darkMode ? "#1b2436" : "#edf3ff";
    const QString cardColor = darkMode ? "#1f2a40" : "#ffffff";
    const QString borderColor = darkMode ? "#2f3f5f" : "#d9e0ee";
    const QString textColor = darkMode ? "#e9f0ff" : "#1f2a44";
    const QString mutedColor = darkMode ? "#9fb0cf" : "#6a7e9f";
    const QString accent = darkMode ? "#68b7ff" : "#356ff5";
    const QString tabNormal = darkMode ? "#22314d" : "#f0f4ff";
    const QString tabHover = darkMode ? "#2c4064" : "#e4ebff";
    const QString tabActive = darkMode ? "#2e4978" : "#dfe8ff";

    setStyleSheet(QString(R"(
QWidget#rootPanel {
    background: qlineargradient(
        x1:0, y1:0, x2:1, y2:1,
        stop:0 %1,
        stop:1 %2
    );
}
QFrame#headerCard,
QFrame#card {
    background: %3;
    border: 1px solid %4;
    border-radius: 14px;
}
QLabel {
    color: %5;
}
QLabel#appName {
    font-size: 22px;
    font-weight: 700;
}
QLabel#welcomeText,
QLabel#scoreText {
    color: %6;
    font-size: 14px;
}
QLabel#moduleTitle {
    font-size: 20px;
    font-weight: 650;
}
QLabel#sectionTitle {
    font-size: 16px;
    font-weight: 600;
}
QLabel#wordLabel {
    font-size: 38px;
    font-weight: 700;
}
QLabel#feedbackText {
    min-height: 26px;
    color: %6;
}
QLabel#progressText {
    font-size: 13px;
}
QLabel#progressHint,
QLabel#moduleDescription {
    color: %6;
    font-size: 13px;
}
QPushButton#primaryButton,
QPushButton#secondaryButton,
QPushButton#ghostButton,
QPushButton#pillButton {
    border-radius: 11px;
    padding: 8px 14px;
    font-weight: 600;
}
QPushButton#primaryButton {
    border: none;
    background: %7;
    color: white;
}
QPushButton#primaryButton:hover {
    background: #4f87f9;
}
QPushButton#secondaryButton,
QPushButton#ghostButton {
    border: 1px solid %4;
    background: transparent;
    color: %5;
}
QPushButton#secondaryButton:hover,
QPushButton#ghostButton:hover {
    background: %9;
}
QPushButton#pillButton {
    border: 1px solid rgba(255, 255, 255, 0.5);
    background: rgba(255, 255, 255, 0.15);
    color: white;
}
QPushButton#pillButton:hover {
    background: rgba(255, 255, 255, 0.25);
}
QPushButton:disabled {
    background: #d5dceb;
    border-color: #d5dceb;
    color: #98a3bc;
}
QRadioButton#optionButton {
    border: 1px solid %4;
    border-radius: 10px;
    padding: 8px 10px;
    background: transparent;
}
QRadioButton#optionButton:hover {
    background: %9;
}
QRadioButton#optionButton::indicator {
    width: 16px;
    height: 16px;
}
QRadioButton#optionButton::indicator:unchecked {
    border: 2px solid %4;
    border-radius: 8px;
    background: transparent;
}
QRadioButton#optionButton::indicator:checked {
    border: 2px solid %7;
    border-radius: 8px;
    background: %7;
}
QProgressBar {
    border: 1px solid %4;
    border-radius: 9px;
    text-align: center;
    background: %8;
    min-height: 18px;
    color: %5;
}
QProgressBar::chunk {
    border-radius: 8px;
    background: %7;
}
QTabWidget#navTabs::pane {
    border: 1px solid %4;
    border-radius: 14px;
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
QTabWidget#navTabs QTabBar::tab:hover {
    background: %9;
}
QTabWidget#navTabs QTabBar::tab:selected {
    color: %5;
    background: %10;
}
)")
        .arg(rootStart, rootEnd, cardColor, borderColor, textColor, mutedColor, accent,
             tabNormal, tabHover, tabActive));
}

void EnglishLearner::updateDailyVisual()
{
    const QDate today = QDate::currentDate();
    labelDate->setText(QStringLiteral("\u4eca\u65e5\u4e3b\u9898 - %1").arg(today.toString("yyyy-MM-dd")));
    labelDailyWord->setText(QStringLiteral("\u4eca\u65e5\u52b1\u5fd7\u8bcd\uff1a%1").arg(generateDailyWord()));
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
    border-radius: 16px;
    background: qlineargradient(
        x1:0, y1:0, x2:1, y2:1,
        stop:0 %1,
        stop:1 %2
    );
}
QFrame#heroFrame QLabel {
    color: white;
}
QLabel#heroDate {
    color: rgba(255, 255, 255, 0.86);
    font-size: 13px;
}
QLabel#dailyWord {
    font-size: 26px;
    font-weight: 700;
}
QLabel#dailyMeaning {
    color: rgba(255, 255, 255, 0.95);
    font-size: 14px;
}
)")
        .arg(startColors[idx], endColors[idx]));
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
        QStringLiteral("\u5c0f\u6b65\u575a\u6301\uff0c\u957f\u671f\u4e00\u5b9a\u6709\u5927\u53d8\u5316\u3002"),
        QStringLiteral("\u7a33\u5b9a\u8282\u594f\uff0c\u6bd4\u77ed\u671f\u7206\u53d1\u66f4\u6709\u6548\u3002"),
        QStringLiteral("\u6bcf\u5929\u590d\u76d8\u4e00\u70b9\u70b9\uff0c\u5b66\u4e60\u6548\u679c\u4f1a\u660e\u663e\u63d0\u5347\u3002"),
        QStringLiteral("\u6562\u5f00\u53e3\uff0c\u624d\u80fd\u628a\u542c\u8bf4\u80fd\u529b\u771f\u6b63\u7ec3\u8d77\u6765\u3002"),
        QStringLiteral("\u5148\u5b8c\u6210\u518d\u4f18\u5316\uff0c\u6bcf\u4e00\u6b21\u7ec3\u4e60\u90fd\u7b97\u6570\u3002"),
        QStringLiteral("\u4fdd\u6301\u597d\u5947\u5fc3\uff0c\u4f60\u4f1a\u66f4\u5feb\u8fdb\u5165\u82f1\u8bed\u8bed\u5883\u3002"),
        QStringLiteral("\u8f93\u5165+\u8f93\u51fa\u53cc\u7ebf\u5e76\u884c\uff0c\u82f1\u8bed\u80fd\u529b\u6210\u957f\u66f4\u5feb\u3002")
    };

    return meanings[QDate::currentDate().dayOfYear() % meanings.size()];
}

void EnglishLearner::updateDailyProgress(int delta)
{
    m_dailyProgress = qBound(0, m_dailyProgress + delta, m_dailyGoal);
    progressDaily->setValue(m_dailyProgress);

    const int reviewGoal = m_dailyGoal / 2;
    const int reviewProgress = qMin(reviewGoal, m_dailyProgress / 2);
    labelProgress->setText(
        QStringLiteral("\u4eca\u65e5\uff1a\u65b0\u5b66%1/%2\uff0c\u590d\u4e60%3/%4")
            .arg(m_dailyProgress)
            .arg(m_dailyGoal)
            .arg(reviewProgress)
            .arg(reviewGoal));
    labelGoal->setText(QStringLiteral("\u5efa\u8bae\uff1a\u5b8c\u6210\u5355\u8bcd\u76ee\u6807\u540e\u53ef\u7ee7\u7eed\u542c\u8bf4\u6216\u9605\u8bfb\u8bad\u7ec3\u3002"));
}

void EnglishLearner::onLoginSuccess(const UserProfile& user)
{
    m_currentUser = user;
    m_dailyProgress = 0;

    setWindowTitle(QStringLiteral("LingoPulse \u5ba2\u6237\u7aef - %1").arg(toDisplayName(m_currentUser.username)));
    labelWelcome->setText(QStringLiteral("\u6b22\u8fce\uff0c%1").arg(toDisplayName(m_currentUser.username)));
    updateScoreDisplay();
    updateDailyProgress(0);
    loadWord();
}

void EnglishLearner::loadWord()
{
    QString errorMessage;
    QuizQuestion question;
    if (!m_apiClient.fetchNextQuestion(&question, &errorMessage)) {
        labelWord->setText(QStringLiteral("\u9898\u76ee\u52a0\u8f7d\u5931\u8d25"));
        setFeedbackText(errorMessage, false);
        btnSubmit->setEnabled(false);
        for (QRadioButton* button : radioButtons) {
            button->setEnabled(false);
            button->setText(QString());
        }
        return;
    }

    m_currentQuestion = question;
    m_currentOptions = question.options;
    labelWord->setText(m_currentQuestion.word);

    for (int i = 0; i < 4; ++i) {
        if (i < m_currentOptions.size()) {
            radioButtons[i]->setText(QString("%1. %2").arg(QChar('A' + i)).arg(m_currentOptions[i]));
            radioButtons[i]->setEnabled(true);
        } else {
            radioButtons[i]->setText(QString());
            radioButtons[i]->setEnabled(false);
        }
    }

    clearSelection();
    setFeedbackText(QString(), false);
    btnSubmit->setEnabled(true);
}

void EnglishLearner::onSubmit()
{
    int selectedIndex = -1;
    for (int i = 0; i < 4; ++i) {
        if (radioButtons[i]->isChecked()) {
            selectedIndex = i;
            break;
        }
    }

    if (selectedIndex < 0 || selectedIndex >= m_currentOptions.size()) {
        QMessageBox::warning(this, QStringLiteral("\u8f93\u5165\u63d0\u793a"), QStringLiteral("\u8bf7\u9009\u62e9\u4e00\u4e2a\u7b54\u6848\u3002"));
        return;
    }

    const QString selectedAnswer = m_currentOptions[selectedIndex];
    QString errorMessage;
    SubmitAnswerResult submitResult;
    btnSubmit->setEnabled(false);

    if (!m_apiClient.submitAnswer(m_currentQuestion.wordId, selectedAnswer, &submitResult, &errorMessage)) {
        btnSubmit->setEnabled(true);
        QMessageBox::warning(this, QStringLiteral("\u63d0\u4ea4\u5931\u8d25"), errorMessage);
        return;
    }

    m_currentUser.totalScore = submitResult.totalScore;
    updateScoreDisplay();
    updateDailyProgress(1);

    if (submitResult.correct) {
        setFeedbackText(QStringLiteral("\u56de\u7b54\u6b63\u786e\uff0c+%1 \u5206").arg(submitResult.scoreDelta), true);
    } else {
        setFeedbackText(QStringLiteral("\u56de\u7b54\u9519\u8bef\uff0c\u6b63\u786e\u7b54\u6848\uff1a%1").arg(submitResult.correctAnswer), false);
    }

    QTimer::singleShot(1200, this, [this]() {
        loadWord();
    });
}

void EnglishLearner::updateScoreDisplay()
{
    labelScore->setText(QStringLiteral("\u79ef\u5206\uff1a%1  \u5b66\u4e60\u5929\u6570\uff1a%2")
                            .arg(m_currentUser.totalScore)
                            .arg(qMax(1, m_currentUser.studyDays)));
}

void EnglishLearner::onShowRanking()
{
    QString errorMessage;
    QList<RankingItem> rankings;
    if (!m_apiClient.fetchRanking(10, &rankings, &errorMessage)) {
        QMessageBox::warning(this, QStringLiteral("\u6392\u884c\u699c"), errorMessage);
        return;
    }

    QDialog rankDialog(this);
    rankDialog.setWindowTitle(QStringLiteral("\u6392\u884c\u699c"));
    rankDialog.resize(460, 360);

    QVBoxLayout* layout = new QVBoxLayout(&rankDialog);

    QTableWidget* table = new QTableWidget(&rankDialog);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(QStringList()
                                     << QStringLiteral("\u6392\u540d")
                                     << QStringLiteral("\u7528\u6237")
                                     << QStringLiteral("\u79ef\u5206"));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setAlternatingRowColors(true);
    table->setRowCount(rankings.size());

    for (int row = 0; row < rankings.size(); ++row) {
        const RankingItem& item = rankings[row];

        QTableWidgetItem* rankItem = new QTableWidgetItem(QString::number(item.rank));
        rankItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, 0, rankItem);

        QTableWidgetItem* userItem = new QTableWidgetItem(toDisplayName(item.username));
        table->setItem(row, 1, userItem);

        QTableWidgetItem* scoreItem = new QTableWidgetItem(QString::number(item.totalScore));
        scoreItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        table->setItem(row, 2, scoreItem);

        if (item.userId == m_currentUser.userId) {
            rankItem->setBackground(QColor("#dff0ff"));
            userItem->setBackground(QColor("#dff0ff"));
            scoreItem->setBackground(QColor("#dff0ff"));
        }
    }

    layout->addWidget(table);

    QPushButton* closeButton = new QPushButton(QStringLiteral("\u5173\u95ed"), &rankDialog);
    closeButton->setObjectName("secondaryButton");
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, &rankDialog, &QDialog::accept);

    rankDialog.exec();
}

void EnglishLearner::onOpenWordBook()
{
    QMessageBox::information(
        this,
        QStringLiteral("\u8bcd\u4e66\u8bbe\u7f6e"),
        QStringLiteral("\u8bcd\u4e66\u5165\u53e3\u5df2\u9884\u7559\uff0c\u53ef\u5728\u540e\u7eed\u7248\u672c\u63a5\u5165\u8bcd\u4e66\u9009\u62e9\u3001\u96be\u5ea6\u5206\u5c42\u4e0e\u76ee\u6807\u8bbe\u7f6e\u3002"));
}

void EnglishLearner::onOpenStudyStatus()
{
    const QString moduleName = navTabs->tabText(navTabs->currentIndex());
    QMessageBox::information(
        this,
        QStringLiteral("\u5b66\u4e60\u72b6\u6001"),
        QStringLiteral("\u5f53\u524d\u677f\u5757\uff1a%1\n\u8be5\u9875\u9762\u5df2\u9884\u7559\uff0c\u53ef\u63a5\u5165\u5b66\u4e60\u66f2\u7ebf\u3001\u5b8c\u6210\u7387\u548c\u9519\u9898\u7edf\u8ba1\u3002")
            .arg(moduleName));
}

QWidget* EnglishLearner::createPlaceholderModule(const QString& moduleName, const QString& description)
{
    QWidget* page = new QWidget(navTabs);
    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(14, 14, 14, 14);
    pageLayout->setSpacing(12);

    QFrame* card = new QFrame(page);
    card->setObjectName("card");
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(16, 16, 16, 16);
    cardLayout->setSpacing(10);

    QHBoxLayout* topLayout = new QHBoxLayout();
    QLabel* title = new QLabel(moduleName, card);
    title->setObjectName("moduleTitle");
    QPushButton* statusButton = new QPushButton(QStringLiteral("\u5b66\u4e60\u72b6\u6001"), card);
    statusButton->setObjectName("ghostButton");
    topLayout->addWidget(title);
    topLayout->addStretch();
    topLayout->addWidget(statusButton);
    cardLayout->addLayout(topLayout);

    QLabel* desc = new QLabel(description, card);
    desc->setObjectName("moduleDescription");
    desc->setWordWrap(true);
    cardLayout->addWidget(desc);

    pageLayout->addWidget(card);
    pageLayout->addStretch();

    connect(statusButton, &QPushButton::clicked, this, &EnglishLearner::onOpenStudyStatus);
    return page;
}

void EnglishLearner::setFeedbackText(const QString& text, bool positive)
{
    labelFeedback->setText(text);
    if (text.isEmpty()) {
        labelFeedback->setStyleSheet("color: #6b7b98;");
        return;
    }

    labelFeedback->setStyleSheet(positive
                                     ? "color: #1c9a57; font-weight: 600;"
                                     : "color: #d14d57; font-weight: 600;");
}

QString EnglishLearner::toDisplayName(const QString& rawUsername) const
{
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
        return QStringLiteral("\u624b\u673a\u53f7(%1)").arg(account);
    }
    if (type == QStringLiteral("wechat")) {
        return QStringLiteral("\u5fae\u4fe1(%1)").arg(account);
    }
    if (type == QStringLiteral("qq")) {
        return QStringLiteral("QQ(%1)").arg(account);
    }
    if (type == QStringLiteral("email")) {
        return QStringLiteral("\u90ae\u7bb1(%1)").arg(account);
    }
    return account;
}

void EnglishLearner::clearSelection()
{
    for (QRadioButton* button : radioButtons) {
        button->setAutoExclusive(false);
        button->setChecked(false);
        button->setAutoExclusive(true);
    }
}
