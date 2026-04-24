#include "LoginDialog.h"

#include <QAbstractButton>
#include <QApplication>
#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>

LoginDialog::LoginDialog(ApiClient* apiClient, QWidget* parent)
    : QDialog(parent)
    , m_apiClient(apiClient)
    , loginTypeGroup(new QButtonGroup(this))
{
    setObjectName("loginDialog");
    setWindowTitle(QStringLiteral("\u6b22\u8fce\u4f7f\u7528LingoPulse"));
    setMinimumSize(560, 460);
    resize(620, 500);

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(28, 28, 28, 28);
    rootLayout->setSpacing(16);

    labelTitle = new QLabel(QStringLiteral("LingoPulse"), this);
    labelTitle->setObjectName("loginTitle");
    labelTitle->setAlignment(Qt::AlignHCenter);

    labelSubtitle = new QLabel(
        QStringLiteral("\u6bcf\u5929\u575a\u6301\u4e00\u70b9\u70b9\uff0c\u82f1\u8bed\u4f1a\u56de\u62a5\u4f60\u770b\u5f97\u89c1\u7684\u6210\u957f\u3002"),
        this);
    labelSubtitle->setObjectName("loginSubtitle");
    labelSubtitle->setAlignment(Qt::AlignHCenter);

    rootLayout->addWidget(labelTitle);
    rootLayout->addWidget(labelSubtitle);

    loginCard = new QFrame(this);
    loginCard->setObjectName("loginCard");
    QVBoxLayout* cardLayout = new QVBoxLayout(loginCard);
    cardLayout->setContentsMargins(20, 20, 20, 20);
    cardLayout->setSpacing(14);

    QLabel* methodTitle = new QLabel(QStringLiteral("\u767b\u5f55\u65b9\u5f0f"), loginCard);
    methodTitle->setObjectName("sectionTitle");
    cardLayout->addWidget(methodTitle);

    QHBoxLayout* methodButtonsLayout = new QHBoxLayout();
    methodButtonsLayout->setSpacing(10);

    const QStringList methodNames{
        QStringLiteral("\u624b\u673a\u53f7"),
        QStringLiteral("\u5fae\u4fe1"),
        QStringLiteral("QQ"),
        QStringLiteral("\u90ae\u7bb1")
    };
    for (int i = 0; i < methodNames.size(); ++i) {
        QPushButton* button = new QPushButton(methodNames[i], loginCard);
        button->setCheckable(true);
        button->setObjectName("loginTypeButton");
        methodButtonsLayout->addWidget(button);
        loginTypeGroup->addButton(button, i);
    }
    loginTypeGroup->button(static_cast<int>(LoginType::Phone))->setChecked(true);
    cardLayout->addLayout(methodButtonsLayout);

    labelTypeHint = new QLabel(loginCard);
    labelTypeHint->setObjectName("typeHint");
    labelTypeHint->setWordWrap(true);
    cardLayout->addWidget(labelTypeHint);

    QGridLayout* formLayout = new QGridLayout();
    formLayout->setHorizontalSpacing(10);
    formLayout->setVerticalSpacing(10);

    QLabel* lblServer = new QLabel(QStringLiteral("\u670d\u52a1\u5730\u5740"), loginCard);
    QLabel* lblAccount = new QLabel(QStringLiteral("\u8d26\u53f7"), loginCard);
    QLabel* lblPassword = new QLabel(QStringLiteral("\u5bc6\u7801"), loginCard);

    lineEditServerUrl = new QLineEdit(loginCard);
    lineEditServerUrl->setPlaceholderText(QStringLiteral("\u4f8b\u5982\uff1ahttp://localhost:8080"));
    lineEditServerUrl->setText(m_apiClient != nullptr ? m_apiClient->baseUrl() : QStringLiteral("http://localhost:8080"));

    lineEditAccount = new QLineEdit(loginCard);
    lineEditPassword = new QLineEdit(loginCard);
    lineEditPassword->setEchoMode(QLineEdit::Password);

    formLayout->addWidget(lblServer, 0, 0);
    formLayout->addWidget(lineEditServerUrl, 0, 1);
    formLayout->addWidget(lblAccount, 1, 0);
    formLayout->addWidget(lineEditAccount, 1, 1);
    formLayout->addWidget(lblPassword, 2, 0);
    formLayout->addWidget(lineEditPassword, 2, 1);
    cardLayout->addLayout(formLayout);

    labelHint = new QLabel(
        QStringLiteral("\u624b\u673a\u53f7\u548c\u90ae\u7bb1\u53ef\u76f4\u63a5\u6ce8\u518c\u767b\u5f55\uff0c\u5fae\u4fe1\u548cQQ\u652f\u6301\u8d26\u53f7\u5165\u53e3\u767b\u5f55\u3002"),
        loginCard);
    labelHint->setObjectName("loginHint");
    labelHint->setWordWrap(true);
    cardLayout->addWidget(labelHint);

    QHBoxLayout* actionLayout = new QHBoxLayout();
    actionLayout->addStretch();

    btnRegister = new QPushButton(QStringLiteral("\u6ce8\u518c"), loginCard);
    btnRegister->setObjectName("secondaryButton");
    btnLogin = new QPushButton(QStringLiteral("\u767b\u5f55"), loginCard);
    btnLogin->setObjectName("primaryButton");
    btnLogin->setDefault(true);

    actionLayout->addWidget(btnRegister);
    actionLayout->addWidget(btnLogin);
    cardLayout->addLayout(actionLayout);

    rootLayout->addWidget(loginCard, 1);

    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(btnRegister, &QPushButton::clicked, this, &LoginDialog::onRegister);
    connect(loginTypeGroup, &QButtonGroup::idClicked, this, &LoginDialog::onSelectLoginType);

    updateLoginTypePresentation();
    applyTheme();
}

LoginDialog::~LoginDialog() = default;

void LoginDialog::applyTheme()
{
    const QColor windowColor = palette().color(QPalette::Window);
    const bool darkMode = windowColor.lightness() < 128;

    const QString dialogStart = darkMode ? "#0f1828" : "#eef3ff";
    const QString dialogEnd = darkMode ? "#1a2136" : "#f7f9ff";
    const QString cardColor = darkMode ? "rgba(20, 29, 44, 0.92)" : "rgba(255, 255, 255, 0.92)";
    const QString borderColor = darkMode ? "#2d3c5a" : "#d7def0";
    const QString textColor = darkMode ? "#ecf3ff" : "#1b2a4b";
    const QString mutedColor = darkMode ? "#9dafcf" : "#65779b";
    const QString buttonNormal = darkMode ? "#202b40" : "#f3f6ff";
    const QString buttonHover = darkMode ? "#2a3956" : "#e7edff";
    const QString accent = darkMode ? "#61b4ff" : "#356ff5";

    setStyleSheet(QString(R"(
QDialog#loginDialog {
    background: qlineargradient(
        x1:0, y1:0, x2:1, y2:1,
        stop:0 %1,
        stop:1 %2
    );
}
QLabel {
    color: %3;
}
QLabel#loginTitle {
    font-size: 30px;
    font-weight: 700;
}
QLabel#loginSubtitle {
    font-size: 15px;
    color: %4;
}
QFrame#loginCard {
    background: %5;
    border: 1px solid %6;
    border-radius: 18px;
}
QLabel#sectionTitle {
    font-size: 17px;
    font-weight: 600;
}
QPushButton#loginTypeButton {
    border: 1px solid %6;
    border-radius: 14px;
    padding: 8px 14px;
    background: %7;
    color: %3;
    font-weight: 500;
}
QPushButton#loginTypeButton:hover {
    background: %8;
}
QPushButton#loginTypeButton:checked {
    background: %9;
    color: #ffffff;
    border: 1px solid %9;
}
QLabel#typeHint,
QLabel#loginHint {
    color: %4;
    font-size: 13px;
}
QLineEdit {
    border: 1px solid %6;
    border-radius: 10px;
    padding: 8px 10px;
    background: %7;
    color: %3;
    selection-background-color: %9;
}
QPushButton#primaryButton,
QPushButton#secondaryButton {
    border-radius: 12px;
    padding: 8px 18px;
    font-weight: 600;
}
QPushButton#primaryButton {
    background: %9;
    color: white;
    border: none;
}
QPushButton#primaryButton:hover {
    background: #4f87f9;
}
QPushButton#secondaryButton {
    background: transparent;
    color: %3;
    border: 1px solid %6;
}
QPushButton#secondaryButton:hover {
    background: %8;
}
QPushButton:disabled {
    color: #9ea7bc;
    background: #d6dbe8;
    border-color: #d6dbe8;
}
)")
        .arg(dialogStart, dialogEnd, textColor, mutedColor, cardColor, borderColor,
             buttonNormal, buttonHover, accent));
}

void LoginDialog::onSelectLoginType(int id)
{
    m_loginType = static_cast<LoginType>(id);
    updateLoginTypePresentation();
}

void LoginDialog::updateLoginTypePresentation()
{
    switch (m_loginType) {
    case LoginType::Phone:
        lineEditAccount->setPlaceholderText(QStringLiteral("\u8bf7\u8f93\u5165\u624b\u673a\u53f7"));
        lineEditPassword->setPlaceholderText(QStringLiteral("\u8bf7\u8f93\u5165\u5bc6\u7801"));
        labelTypeHint->setText(QStringLiteral("\u624b\u673a\u53f7\u65b9\u5f0f\u5df2\u63a5\u5165\uff0c\u652f\u6301\u8d26\u53f7\u5bc6\u7801\u767b\u5f55\u3002"));
        break;
    case LoginType::Wechat:
        lineEditAccount->setPlaceholderText(QStringLiteral("\u8bf7\u8f93\u5165\u5fae\u4fe1\u53f7"));
        lineEditPassword->setPlaceholderText(QStringLiteral("\u8bf7\u8f93\u5165\u5bc6\u7801"));
        labelTypeHint->setText(QStringLiteral("\u5fae\u4fe1\u65b9\u5f0f\u5df2\u63a5\u5165\uff0c\u652f\u6301\u8d26\u53f7\u5bc6\u7801\u767b\u5f55\u3002"));
        break;
    case LoginType::QQ:
        lineEditAccount->setPlaceholderText(QStringLiteral("\u8bf7\u8f93\u5165QQ\u53f7"));
        lineEditPassword->setPlaceholderText(QStringLiteral("\u8bf7\u8f93\u5165\u5bc6\u7801"));
        labelTypeHint->setText(QStringLiteral("QQ\u65b9\u5f0f\u5df2\u63a5\u5165\uff0c\u652f\u6301\u8d26\u53f7\u5bc6\u7801\u767b\u5f55\u3002"));
        break;
    case LoginType::Email:
        lineEditAccount->setPlaceholderText(QStringLiteral("\u8bf7\u8f93\u5165\u90ae\u7bb1\u5730\u5740"));
        lineEditPassword->setPlaceholderText(QStringLiteral("\u8bf7\u8f93\u5165\u5bc6\u7801"));
        labelTypeHint->setText(QStringLiteral("\u90ae\u7bb1\u65b9\u5f0f\u5df2\u63a5\u5165\uff0c\u652f\u6301\u8d26\u53f7\u5bc6\u7801\u767b\u5f55\u3002"));
        break;
    }
}

void LoginDialog::onLogin()
{
    if (m_apiClient == nullptr) {
        QMessageBox::critical(this, QStringLiteral("\u9519\u8bef"), QStringLiteral("API\u5ba2\u6237\u7aef\u672a\u521d\u59cb\u5316\u3002"));
        return;
    }

    QString account;
    QString password;
    if (!validateInput(&account, &password)) {
        return;
    }

    const QString serverUrl = lineEditServerUrl->text().trimmed();
    m_apiClient->setBaseUrl(serverUrl);

    UserProfile profile;
    QString token;
    QString errorMessage;
    const QString backendAccount = toBackendAccount(account);

    setBusy(true);
    bool ok = m_apiClient->login(backendAccount, password, &profile, &token, &errorMessage);
    if (!ok && backendAccount != account) {
        QString fallbackError;
        if (m_apiClient->login(account, password, &profile, &token, &fallbackError)) {
            ok = true;
        }
    }
    setBusy(false);

    if (!ok) {
        const QString message = QStringLiteral("%1\u767b\u5f55\u5931\u8d25\uff1a%2").arg(currentTypeName(), errorMessage);
        QMessageBox::warning(this, QStringLiteral("\u767b\u5f55\u5931\u8d25"), message);
        return;
    }

    Q_UNUSED(token);
    m_loggedInUser = profile;
    accept();
}

void LoginDialog::onRegister()
{
    if (m_apiClient == nullptr) {
        QMessageBox::critical(this, QStringLiteral("\u9519\u8bef"), QStringLiteral("API\u5ba2\u6237\u7aef\u672a\u521d\u59cb\u5316\u3002"));
        return;
    }

    QString account;
    QString password;
    if (!validateInput(&account, &password)) {
        return;
    }

    const QString backendAccount = toBackendAccount(account);
    if (backendAccount.size() < 2 || backendAccount.size() > 64) {
        QMessageBox::warning(this, QStringLiteral("\u8f93\u5165\u63d0\u793a"), QStringLiteral("\u8d26\u53f7\u957f\u5ea6\u9700\u57282\u523064\u4e4b\u95f4\u3002"));
        return;
    }
    if (password.size() < 6 || password.size() > 64) {
        QMessageBox::warning(this, QStringLiteral("\u8f93\u5165\u63d0\u793a"), QStringLiteral("\u5bc6\u7801\u957f\u5ea6\u9700\u57286\u523064\u4e4b\u95f4\u3002"));
        return;
    }

    const QString serverUrl = lineEditServerUrl->text().trimmed();
    m_apiClient->setBaseUrl(serverUrl);

    QString errorMessage;
    setBusy(true);
    const bool ok = m_apiClient->registerUser(backendAccount, password, &errorMessage);
    setBusy(false);

    if (!ok) {
        QMessageBox::warning(this, QStringLiteral("\u6ce8\u518c\u5931\u8d25"), errorMessage);
        return;
    }

    QMessageBox::information(this, QStringLiteral("\u6ce8\u518c\u6210\u529f"), QStringLiteral("\u8d26\u53f7\u5df2\u521b\u5efa\uff0c\u8bf7\u76f4\u63a5\u767b\u5f55\u3002"));
    lineEditPassword->clear();
}

bool LoginDialog::validateInput(QString* account, QString* password) const
{
    const QString serverUrl = lineEditServerUrl->text().trimmed();
    if (serverUrl.isEmpty()) {
        QMessageBox::warning(const_cast<LoginDialog*>(this), QStringLiteral("\u8f93\u5165\u63d0\u793a"), QStringLiteral("\u8bf7\u8f93\u5165\u670d\u52a1\u5730\u5740\u3002"));
        return false;
    }

    const QString inputAccount = lineEditAccount->text().trimmed();
    const QString inputPassword = lineEditPassword->text();

    if (inputAccount.isEmpty() || inputPassword.isEmpty()) {
        QMessageBox::warning(const_cast<LoginDialog*>(this), QStringLiteral("\u8f93\u5165\u63d0\u793a"), QStringLiteral("\u8d26\u53f7\u548c\u5bc6\u7801\u4e0d\u80fd\u4e3a\u7a7a\u3002"));
        return false;
    }

    if (m_loginType == LoginType::Phone) {
        const QRegularExpression phonePattern("^\\+?[0-9\\- ]{6,20}$");
        if (!phonePattern.match(inputAccount).hasMatch()) {
            QMessageBox::warning(const_cast<LoginDialog*>(this),
                                 QStringLiteral("\u8f93\u5165\u63d0\u793a"),
                                 QStringLiteral("\u624b\u673a\u53f7\u683c\u5f0f\u4e0d\u6b63\u786e\uff0c\u8bf7\u8f93\u51656-20\u4f4d\u6570\u5b57\uff08\u53ef\u542b+\u3001-\u3001\u7a7a\u683c\uff09\u3002"));
            return false;
        }
    } else if (m_loginType == LoginType::Wechat) {
        const QRegularExpression wechatPattern("^[a-zA-Z][-_a-zA-Z0-9]{5,19}$");
        if (!wechatPattern.match(inputAccount).hasMatch()) {
            QMessageBox::warning(const_cast<LoginDialog*>(this),
                                 QStringLiteral("\u8f93\u5165\u63d0\u793a"),
                                 QStringLiteral("\u5fae\u4fe1\u53f7\u683c\u5f0f\u4e0d\u6b63\u786e\uff0c\u8bf7\u4f7f\u75286-20\u4f4d\u5b57\u6bcd\u6216\u6570\u5b57\u7ec4\u5408\u3002"));
            return false;
        }
    } else if (m_loginType == LoginType::QQ) {
        const QRegularExpression qqPattern("^[1-9][0-9]{4,11}$");
        if (!qqPattern.match(inputAccount).hasMatch()) {
            QMessageBox::warning(const_cast<LoginDialog*>(this),
                                 QStringLiteral("\u8f93\u5165\u63d0\u793a"),
                                 QStringLiteral("QQ\u53f7\u683c\u5f0f\u4e0d\u6b63\u786e\uff0c\u8bf7\u8f93\u51655-12\u4f4d\u6570\u5b57\u3002"));
            return false;
        }
    } else if (m_loginType == LoginType::Email && !inputAccount.contains('@')) {
        QMessageBox::warning(const_cast<LoginDialog*>(this), QStringLiteral("\u8f93\u5165\u63d0\u793a"), QStringLiteral("\u90ae\u7bb1\u683c\u5f0f\u4e0d\u6b63\u786e\uff0c\u8bf7\u91cd\u65b0\u8f93\u5165\u3002"));
        return false;
    }

    if (account != nullptr) {
        *account = normalizeAccount(inputAccount);
    }
    if (password != nullptr) {
        *password = inputPassword;
    }
    return true;
}

QString LoginDialog::toBackendAccount(const QString& rawAccount) const
{
    const QString normalized = normalizeAccount(rawAccount);
    switch (m_loginType) {
    case LoginType::Phone:
        return QStringLiteral("phone:%1").arg(normalized);
    case LoginType::Wechat:
        return QStringLiteral("wechat:%1").arg(normalized);
    case LoginType::QQ:
        return QStringLiteral("qq:%1").arg(normalized);
    case LoginType::Email:
        return QStringLiteral("email:%1").arg(normalized);
    }
    return QStringLiteral("account:%1").arg(normalized);
}

QString LoginDialog::normalizeAccount(const QString& rawAccount) const
{
    QString normalized = rawAccount.trimmed();
    if (m_loginType == LoginType::Phone) {
        normalized.remove(' ');
        normalized.remove('-');
    } else if (m_loginType == LoginType::Email) {
        normalized = normalized.toLower();
    }
    return normalized;
}

QString LoginDialog::currentTypeName() const
{
    switch (m_loginType) {
    case LoginType::Phone:
        return QStringLiteral("\u624b\u673a\u53f7");
    case LoginType::Wechat:
        return QStringLiteral("\u5fae\u4fe1");
    case LoginType::QQ:
        return QStringLiteral("QQ");
    case LoginType::Email:
        return QStringLiteral("\u90ae\u7bb1");
    }
    return QStringLiteral("\u8d26\u53f7");
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
    lineEditAccount->setEnabled(!busy);
    lineEditPassword->setEnabled(!busy);
    for (QAbstractButton* button : loginTypeGroup->buttons()) {
        button->setEnabled(!busy);
    }

    if (busy) {
        QApplication::setOverrideCursor(Qt::BusyCursor);
    } else {
        QApplication::restoreOverrideCursor();
    }
}
