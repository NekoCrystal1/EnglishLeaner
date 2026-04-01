#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>
#include "ui_EnglishLearner.h"

class EnglishLearner : public QMainWindow
{
    Q_OBJECT

public:
    EnglishLearner(QWidget* parent = nullptr);
    ~EnglishLearner();

private slots:
    void onLoginSuccess(int userId, QString username);
    void onSubmit();             // 提交答案
    void onShowRanking();        // 显示排行榜

private:
    void loadWord();             // 从数据库加载单词
    void updateScoreDisplay();   // 更新得分显示

private:
    Ui::EnglishLearnerClass ui;

    // UI控件（纯代码方式）
    QLabel* labelWelcome;
    QLabel* labelScore;
    QLabel* labelWord;
    QRadioButton* radioButtons[4];
    QPushButton* btnSubmit;
    QPushButton* btnRanking;
    QLabel* labelFeedback;

    // 用户信息
    int currentUserId;
    QString currentUsername;
    int currentScore;

    // 当前单词信息
    int currentWordId;
    QString currentWord;
    QString currentTranslation;
    QStringList options;         // 四个选项（包含正确和错误）
};