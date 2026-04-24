#pragma once
#include <QDialog>

#include "ApiClient.h"

class QButtonGroup;
class QFrame;
class QLineEdit;
class QPushButton;
class QLabel;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(ApiClient* apiClient, QWidget* parent = nullptr);
    ~LoginDialog() override;

    UserProfile loggedInUser() const;

private slots:
    void onLogin();
    void onRegister();
    void onOfflineDemo();
    void onSelectLoginType(int id);

private:
    enum class LoginType {
        Phone = 0,
        Wechat = 1,
        QQ = 2,
        Email = 3
    };

    void applyTheme();
    void updateLoginTypePresentation();
    bool validateInput(QString* account, QString* password) const;
    QString toBackendAccount(const QString& rawAccount) const;
    QString normalizeAccount(const QString& rawAccount) const;
    QString currentTypeName() const;
    void setBusy(bool busy);

private:
    ApiClient* m_apiClient;
    UserProfile m_loggedInUser;
    LoginType m_loginType = LoginType::Phone;

    QButtonGroup* loginTypeGroup;
    QFrame* loginCard;
    QLineEdit* lineEditServerUrl;
    QLineEdit* lineEditAccount;
    QLineEdit* lineEditPassword;
    QPushButton* btnLogin;
    QPushButton* btnRegister;
    QPushButton* btnOfflineDemo;
    QLabel* labelTitle;
    QLabel* labelSubtitle;
    QLabel* labelTypeHint;
    QLabel* labelHint;
};
