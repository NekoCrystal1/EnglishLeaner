#include "EnglishLearner.h"
#include "LoginDialog.h"
#include <QtWidgets>
#include <QtSql>
#include <QTimer>
#include <QMessageBox>
#include <random>
#include <algorithm>

EnglishLearner::EnglishLearner(QWidget* parent)
    : QMainWindow(parent)
    , currentUserId(0)
    , currentScore(0)
    , currentWordId(0)
{
    // 设置窗口属性
    setWindowTitle("英语自学系统");
    resize(600, 500);

    // 创建中心控件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // 顶部信息栏
    QHBoxLayout* topLayout = new QHBoxLayout();
    labelWelcome = new QLabel("欢迎", this);
    labelScore = new QLabel("积分: 0", this);
    topLayout->addWidget(labelWelcome);
    topLayout->addStretch();
    topLayout->addWidget(labelScore);
    mainLayout->addLayout(topLayout);

    // 单词显示区域
    labelWord = new QLabel(this);
    labelWord->setAlignment(Qt::AlignCenter);
    QFont wordFont;
    wordFont.setPointSize(24);
    wordFont.setBold(true);
    labelWord->setFont(wordFont);
    labelWord->setMinimumHeight(100);
    mainLayout->addWidget(labelWord);

    // 选项区域
    QVBoxLayout* optionsLayout = new QVBoxLayout();
    for (int i = 0; i < 4; i++) {
        radioButtons[i] = new QRadioButton(this);
        optionsLayout->addWidget(radioButtons[i]);
    }
    mainLayout->addLayout(optionsLayout);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    btnSubmit = new QPushButton("提交答案", this);
    btnRanking = new QPushButton("排行榜", this);
    buttonLayout->addWidget(btnSubmit);
    buttonLayout->addWidget(btnRanking);
    mainLayout->addLayout(buttonLayout);

    // 反馈标签
    labelFeedback = new QLabel(this);
    labelFeedback->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(labelFeedback);

    // 连接信号槽
    connect(btnSubmit, &QPushButton::clicked, this, &EnglishLearner::onSubmit);
    connect(btnRanking, &QPushButton::clicked, this, &EnglishLearner::onShowRanking);

    // 先显示登录对话框
    LoginDialog* loginDlg = new LoginDialog(this);
    connect(loginDlg, &LoginDialog::loginSuccess, this, &EnglishLearner::onLoginSuccess);

    if (loginDlg->exec() == QDialog::Accepted) {
        delete loginDlg;
    }
    else {
        close();
    }
}

EnglishLearner::~EnglishLearner()
{
}

void EnglishLearner::onLoginSuccess(int userId, QString username)
{
    currentUserId = userId;
    currentUsername = username;

    setWindowTitle(QString("英语自学系统 - %1").arg(username));
    labelWelcome->setText(QString("欢迎，%1").arg(username));

    // 获取用户当前积分
    QSqlQuery query;
    query.prepare("SELECT total_score FROM users WHERE user_id = ?");
    query.addBindValue(currentUserId);
    if (query.exec() && query.next()) {
        currentScore = query.value(0).toInt();
        updateScoreDisplay();
    }

    // 加载第一个单词
    loadWord();
}

void EnglishLearner::loadWord()
{
    // SQLite使用 RANDOM() 随机获取一个单词
    QSqlQuery query("SELECT word_id, word, translation FROM vocabulary ORDER BY RANDOM() LIMIT 1");

    if (query.exec() && query.next()) {
        currentWordId = query.value(0).toInt();
        currentWord = query.value(1).toString();
        currentTranslation = query.value(2).toString();

        // 显示单词
        labelWord->setText(currentWord);

        // 获取选项：正确翻译 + 3个随机其他单词的翻译
        options.clear();
        options << currentTranslation;

        QSqlQuery otherQuery;
        otherQuery.prepare("SELECT translation FROM vocabulary WHERE word_id != ? ORDER BY RANDOM() LIMIT 3");
        otherQuery.addBindValue(currentWordId);
        otherQuery.exec();

        while (otherQuery.next()) {
            QString trans = otherQuery.value(0).toString();
            if (!options.contains(trans)) {
                options << trans;
            }
        }

        // 如果不够4个选项，补默认值
        while (options.size() < 4) {
            options << "未知";
        }

        // 随机打乱选项顺序
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(options.begin(), options.end(), g);

        // 更新界面
        for (int i = 0; i < 4; i++) {
            if (i < options.size()) {
                radioButtons[i]->setText(QString("%1. %2").arg(QChar('A' + i)).arg(options[i]));
                radioButtons[i]->setEnabled(true);
            }
        }

        // 清空反馈
        labelFeedback->setText("");

        // 清空选项
        for (int i = 0; i < 4; i++) {
            radioButtons[i]->setAutoExclusive(false);
            radioButtons[i]->setChecked(false);
            radioButtons[i]->setAutoExclusive(true);
        }
    }
    else {
        labelWord->setText("暂无单词数据");
        qDebug() << "加载单词失败:" << query.lastError().text();
    }
}

void EnglishLearner::onSubmit()
{
    // 获取选中的选项
    int selectedIndex = -1;
    for (int i = 0; i < 4; i++) {
        if (radioButtons[i]->isChecked()) {
            selectedIndex = i;
            break;
        }
    }

    if (selectedIndex == -1) {
        QMessageBox::warning(this, "提示", "请选择一个答案");
        return;
    }

    QString selectedAnswer = options[selectedIndex];
    bool isCorrect = (selectedAnswer == currentTranslation);

    // 记录学习记录
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO study_log (user_id, word_id, is_correct) VALUES (?, ?, ?)");
    logQuery.addBindValue(currentUserId);
    logQuery.addBindValue(currentWordId);
    logQuery.addBindValue(isCorrect ? 1 : 0);

    if (!logQuery.exec()) {
        qDebug() << "记录学习记录失败:" << logQuery.lastError().text();
    }

    // 如果正确，增加积分
    if (isCorrect) {
        currentScore += 10;
        updateScoreDisplay();

        // 更新数据库中的积分
        QSqlQuery updateScore;
        updateScore.prepare("UPDATE users SET total_score = ? WHERE user_id = ?");
        updateScore.addBindValue(currentScore);
        updateScore.addBindValue(currentUserId);

        if (!updateScore.exec()) {
            qDebug() << "更新积分失败:" << updateScore.lastError().text();
        }

        labelFeedback->setText("✓ 回答正确！ +10分");
        labelFeedback->setStyleSheet("color: green; font-weight: bold;");
    }
    else {
        labelFeedback->setText(QString("✗ 回答错误！正确答案是：%1").arg(currentTranslation));
        labelFeedback->setStyleSheet("color: red; font-weight: bold;");
    }

    // 延迟后加载下一个单词
    QTimer::singleShot(1500, this, &EnglishLearner::loadWord);
}

void EnglishLearner::updateScoreDisplay()
{
    labelScore->setText(QString("积分: %1").arg(currentScore));
}

void EnglishLearner::onShowRanking()
{
    // 创建排行榜对话框
    QDialog* rankDialog = new QDialog(this);
    rankDialog->setWindowTitle("积分排行榜");
    rankDialog->setMinimumSize(400, 300);

    QVBoxLayout* layout = new QVBoxLayout(rankDialog);

    QTableWidget* table = new QTableWidget(rankDialog);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(QStringList() << "排名" << "用户名" << "积分");
    table->horizontalHeader()->setStretchLastSection(true);

    // SQLite使用 LIMIT 10
    QSqlQuery query("SELECT username, total_score FROM users ORDER BY total_score DESC LIMIT 10");
    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        QTableWidgetItem* rankItem = new QTableWidgetItem(QString::number(row + 1));
        rankItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, 0, rankItem);

        QTableWidgetItem* nameItem = new QTableWidgetItem(query.value(0).toString());
        table->setItem(row, 1, nameItem);

        QTableWidgetItem* scoreItem = new QTableWidgetItem(QString::number(query.value(1).toInt()));
        scoreItem->setTextAlignment(Qt::AlignRight);
        table->setItem(row, 2, scoreItem);
        row++;
    }

    layout->addWidget(table);

    QPushButton* btnClose = new QPushButton("关闭", rankDialog);
    layout->addWidget(btnClose);

    connect(btnClose, &QPushButton::clicked, rankDialog, &QDialog::accept);

    rankDialog->exec();
    delete rankDialog;
}