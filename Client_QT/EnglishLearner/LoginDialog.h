#pragma once
#include <QDialog>

#include "ApiClient.h"

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

private:
    void setBusy(bool busy);

private:
    ApiClient* m_apiClient;
    UserProfile m_loggedInUser;

    QLineEdit* lineEditServerUrl;
    QLineEdit* lineEditUsername;
    QLineEdit* lineEditPassword;
    QPushButton* btnLogin;
    QPushButton* btnRegister;
    QLabel* labelHint;
};
