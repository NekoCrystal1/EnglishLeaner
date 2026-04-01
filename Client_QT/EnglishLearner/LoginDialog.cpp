#include "LoginDialog.h"
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <qsqlerror.h>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("登录");
    setFixedSize(300, 200);

    // 创建控件
    QLabel* lblUsername = new QLabel("用户名:", this);
    QLabel* lblPassword = new QLabel("密码:", this);
    lineEditUsername = new QLineEdit(this);
    lineEditPassword = new QLineEdit(this);
    lineEditPassword->setEchoMode(QLineEdit::Password);
    btnLogin = new QPushButton("登录", this);
    btnRegister = new QPushButton("注册", this);

    // 布局
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->addWidget(lblUsername, 0, 0);
    gridLayout->addWidget(lineEditUsername, 0, 1);
    gridLayout->addWidget(lblPassword, 1, 0);
    gridLayout->addWidget(lineEditPassword, 1, 1);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnLogin);
    btnLayout->addWidget(btnRegister);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(btnLayout);

    // 连接信号槽
    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(btnRegister, &QPushButton::clicked, this, &LoginDialog::onRegister);
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::onLogin()
{
    QString username = lineEditUsername->text().trimmed();
    QString password = lineEditPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT user_id, username FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (query.exec() && query.next()) {
        int userId = query.value(0).toInt();
        QString userName = query.value(1).toString();
        emit loginSuccess(userId, userName);
        accept();
    }
    else {
        QMessageBox::warning(this, "错误", "用户名或密码错误");
    }
}

void LoginDialog::onRegister()
{
    QString username = lineEditUsername->text().trimmed();
    QString password = lineEditPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空");
        return;
    }

    QSqlQuery check;
    check.prepare("SELECT user_id FROM users WHERE username = ?");
    check.addBindValue(username);
    check.exec();

    if (check.next()) {
        QMessageBox::warning(this, "错误", "用户名已存在");
        return;
    }

    QSqlQuery insert;
    insert.prepare("INSERT INTO users (username, password) VALUES (?, ?)");
    insert.addBindValue(username);
    insert.addBindValue(password);

    if (insert.exec()) {
        QMessageBox::information(this, "成功", "注册成功，请登录");
        lineEditUsername->clear();
        lineEditPassword->clear();
    }
    else {
        QMessageBox::critical(this, "错误", "注册失败：" + insert.lastError().text());
    }
}