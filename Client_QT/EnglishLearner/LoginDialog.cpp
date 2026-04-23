#include "LoginDialog.h"

#include <QApplication>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

LoginDialog::LoginDialog(ApiClient* apiClient, QWidget* parent)
    : QDialog(parent)
    , m_apiClient(apiClient)
{
    setWindowTitle("登录 / 注册");
    setFixedSize(420, 230);

    QLabel* lblServer = new QLabel("服务地址:", this);
    QLabel* lblUsername = new QLabel("用户名:", this);
    QLabel* lblPassword = new QLabel("密码:", this);

    lineEditServerUrl = new QLineEdit(this);
    lineEditServerUrl->setPlaceholderText("例如: http://localhost:8080");
    lineEditServerUrl->setText(m_apiClient != nullptr ? m_apiClient->baseUrl() : "http://localhost:8080");

    lineEditUsername = new QLineEdit(this);
    lineEditPassword = new QLineEdit(this);
    lineEditPassword->setEchoMode(QLineEdit::Password);
    lineEditPassword->setPlaceholderText("至少 6 位");

    btnLogin = new QPushButton("登录", this);
    btnRegister = new QPushButton("注册", this);
    btnLogin->setDefault(true);

    labelHint = new QLabel("先注册再登录即可开始练习。", this);
    labelHint->setStyleSheet("color: #555;");
    labelHint->setWordWrap(true);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(lblServer, lineEditServerUrl);
    formLayout->addRow(lblUsername, lineEditUsername);
    formLayout->addRow(lblPassword, lineEditPassword);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btnLogin);
    btnLayout->addWidget(btnRegister);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(labelHint);
    mainLayout->addLayout(btnLayout);

    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(btnRegister, &QPushButton::clicked, this, &LoginDialog::onRegister);
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::onLogin()
{
    if (m_apiClient == nullptr) {
        QMessageBox::critical(this, "错误", "API 客户端未初始化");
        return;
    }

    const QString serverUrl = lineEditServerUrl->text().trimmed();
    const QString username = lineEditUsername->text().trimmed();
    const QString password = lineEditPassword->text();

    if (serverUrl.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入服务地址");
        return;
    }
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空");
        return;
    }

    m_apiClient->setBaseUrl(serverUrl);

    UserProfile profile;
    QString token;
    QString errorMessage;

    setBusy(true);
    const bool ok = m_apiClient->login(username, password, &profile, &token, &errorMessage);
    setBusy(false);

    if (!ok) {
        QMessageBox::warning(this, "登录失败", errorMessage);
        return;
    }

    Q_UNUSED(token);
    m_loggedInUser = profile;
    accept();
}

void LoginDialog::onRegister()
{
    if (m_apiClient == nullptr) {
        QMessageBox::critical(this, "错误", "API 客户端未初始化");
        return;
    }

    const QString serverUrl = lineEditServerUrl->text().trimmed();
    const QString username = lineEditUsername->text().trimmed();
    const QString password = lineEditPassword->text();

    if (serverUrl.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入服务地址");
        return;
    }
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空");
        return;
    }
    if (username.size() < 2 || username.size() > 32) {
        QMessageBox::warning(this, "提示", "用户名长度需在 2~32 之间");
        return;
    }
    if (password.size() < 6 || password.size() > 64) {
        QMessageBox::warning(this, "提示", "密码长度需在 6~64 之间");
        return;
    }

    m_apiClient->setBaseUrl(serverUrl);

    QString errorMessage;
    setBusy(true);
    const bool ok = m_apiClient->registerUser(username, password, &errorMessage);
    setBusy(false);

    if (!ok) {
        QMessageBox::warning(this, "注册失败", errorMessage);
        return;
    }

    QMessageBox::information(this, "注册成功", "账号已创建，请直接登录。");
    lineEditPassword->clear();
}

UserProfile LoginDialog::loggedInUser() const
{
    return m_loggedInUser;
}

void LoginDialog::setBusy(bool busy)
{
    btnLogin->setEnabled(!busy);
    btnRegister->setEnabled(!busy);
    lineEditServerUrl->setEnabled(!busy);
    lineEditUsername->setEnabled(!busy);
    lineEditPassword->setEnabled(!busy);

    if (busy) {
        QApplication::setOverrideCursor(Qt::BusyCursor);
    } else {
        QApplication::restoreOverrideCursor();
    }
}
