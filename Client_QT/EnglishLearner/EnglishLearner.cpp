#include "EnglishLearner.h"

#include "LoginDialog.h"

#include <QAbstractItemView>
#include <QColor>
#include <QDialog>
#include <QFont>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>

EnglishLearner::EnglishLearner(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("英语自学系统");
    resize(640, 520);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout* topLayout = new QHBoxLayout();
    labelWelcome = new QLabel("未登录", this);
    labelScore = new QLabel("积分: 0", this);
    topLayout->addWidget(labelWelcome);
    topLayout->addStretch();
    topLayout->addWidget(labelScore);
    mainLayout->addLayout(topLayout);

    labelWord = new QLabel("请先登录", this);
    labelWord->setAlignment(Qt::AlignCenter);
    QFont wordFont;
    wordFont.setPointSize(24);
    wordFont.setBold(true);
    labelWord->setFont(wordFont);
    labelWord->setMinimumHeight(110);
    mainLayout->addWidget(labelWord);

    QVBoxLayout* optionsLayout = new QVBoxLayout();
    for (int i = 0; i < 4; ++i) {
        radioButtons[i] = new QRadioButton(this);
        radioButtons[i]->setEnabled(false);
        optionsLayout->addWidget(radioButtons[i]);
    }
    mainLayout->addLayout(optionsLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    btnSubmit = new QPushButton("提交答案", this);
    btnRanking = new QPushButton("排行榜", this);
    btnSubmit->setEnabled(false);
    buttonLayout->addWidget(btnSubmit);
    buttonLayout->addWidget(btnRanking);
    mainLayout->addLayout(buttonLayout);

    labelFeedback = new QLabel(this);
    labelFeedback->setAlignment(Qt::AlignCenter);
    labelFeedback->setWordWrap(true);
    mainLayout->addWidget(labelFeedback);

    connect(btnSubmit, &QPushButton::clicked, this, &EnglishLearner::onSubmit);
    connect(btnRanking, &QPushButton::clicked, this, &EnglishLearner::onShowRanking);

    LoginDialog loginDlg(&m_apiClient, this);
    if (loginDlg.exec() != QDialog::Accepted) {
        QTimer::singleShot(0, this, &QWidget::close);
        return;
    }

    onLoginSuccess(loginDlg.loggedInUser());
}

EnglishLearner::~EnglishLearner() = default;

void EnglishLearner::onLoginSuccess(const UserProfile& user)
{
    m_currentUser = user;

    setWindowTitle(QString("英语自学系统 - %1").arg(m_currentUser.username));
    labelWelcome->setText(QString("欢迎，%1").arg(m_currentUser.username));
    updateScoreDisplay();

    loadWord();
}

void EnglishLearner::loadWord()
{
    QString errorMessage;
    QuizQuestion question;
    if (!m_apiClient.fetchNextQuestion(&question, &errorMessage)) {
        labelWord->setText("加载题目失败");
        labelFeedback->setText(errorMessage);
        labelFeedback->setStyleSheet("color: #d23; font-weight: bold;");
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
    labelFeedback->clear();
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
        QMessageBox::warning(this, "提示", "请选择一个答案");
        return;
    }

    const QString selectedAnswer = m_currentOptions[selectedIndex];

    QString errorMessage;
    SubmitAnswerResult submitResult;
    btnSubmit->setEnabled(false);

    if (!m_apiClient.submitAnswer(m_currentQuestion.wordId, selectedAnswer, &submitResult, &errorMessage)) {
        btnSubmit->setEnabled(true);
        QMessageBox::warning(this, "提交失败", errorMessage);
        return;
    }

    m_currentUser.totalScore = submitResult.totalScore;
    updateScoreDisplay();

    if (submitResult.correct) {
        labelFeedback->setText(QString("✓ 回答正确！ +%1分").arg(submitResult.scoreDelta));
        labelFeedback->setStyleSheet("color: #1f8b4c; font-weight: bold;");
    } else {
        labelFeedback->setText(QString("✗ 回答错误！正确答案是：%1").arg(submitResult.correctAnswer));
        labelFeedback->setStyleSheet("color: #d23; font-weight: bold;");
    }

    QTimer::singleShot(1200, this, [this]() {
        loadWord();
    });
}

void EnglishLearner::updateScoreDisplay()
{
    labelScore->setText(QString("积分: %1").arg(m_currentUser.totalScore));
}

void EnglishLearner::onShowRanking()
{
    QString errorMessage;
    QList<RankingItem> rankings;
    if (!m_apiClient.fetchRanking(10, &rankings, &errorMessage)) {
        QMessageBox::warning(this, "排行榜", errorMessage);
        return;
    }

    QDialog rankDialog(this);
    rankDialog.setWindowTitle("积分排行榜");
    rankDialog.resize(430, 340);

    QVBoxLayout* layout = new QVBoxLayout(&rankDialog);

    QTableWidget* table = new QTableWidget(&rankDialog);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(QStringList() << "排名" << "用户名" << "积分");
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setRowCount(rankings.size());

    for (int row = 0; row < rankings.size(); ++row) {
        const RankingItem& item = rankings[row];

        QTableWidgetItem* rankItem = new QTableWidgetItem(QString::number(item.rank));
        rankItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, 0, rankItem);

        QTableWidgetItem* userItem = new QTableWidgetItem(item.username);
        table->setItem(row, 1, userItem);

        QTableWidgetItem* scoreItem = new QTableWidgetItem(QString::number(item.totalScore));
        scoreItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        table->setItem(row, 2, scoreItem);

        if (item.userId == m_currentUser.userId) {
            rankItem->setBackground(QColor("#E8F4FF"));
            userItem->setBackground(QColor("#E8F4FF"));
            scoreItem->setBackground(QColor("#E8F4FF"));
        }
    }

    layout->addWidget(table);

    QPushButton* closeButton = new QPushButton("关闭", &rankDialog);
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, &rankDialog, &QDialog::accept);

    rankDialog.exec();
}

void EnglishLearner::clearSelection()
{
    for (QRadioButton* button : radioButtons) {
        button->setAutoExclusive(false);
        button->setChecked(false);
        button->setAutoExclusive(true);
    }
}
