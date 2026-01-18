#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_registerButton_clicked();
    void on_loginButton_clicked();
    void onLoginReply(QNetworkReply* reply);
    void onRegisterReply(QNetworkReply* reply);

private:
    void setupUiElements();
    void executeAuthRequest(const QString& url);

    Ui::MainWindow* m_ui;
    QNetworkAccessManager* m_networkManager;

    const QString m_apiLoginUrl = "http://localhost:18080/api/login";
    const QString m_apiRegisterUrl = "http://localhost:18080/api/register";
    const QString m_logoPath = ":/LOGO.png";

    const QString m_jsonUser = "username";
    const QString m_jsonPass = "password";
    const QString m_jsonSuccess = "success";
    const QString m_jsonUserId = "userId";
    const QString m_contentType = "application/json";

    const QString m_msgRegSuccessTitle = "Success";
    const QString m_msgRegSuccessBody = "Registration successful. You can log in now.";
};
#endif