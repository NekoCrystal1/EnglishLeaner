#pragma once
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog();

signals:
    void loginSuccess(int userId, QString username);

private slots:
    void onLogin();
    void onRegister();

private:
    QLineEdit* lineEditUsername;
    QLineEdit* lineEditPassword;
    QPushButton* btnLogin;
    QPushButton* btnRegister;
};