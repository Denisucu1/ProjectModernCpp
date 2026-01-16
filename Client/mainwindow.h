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

    Ui::MainWindow* ui;
    QNetworkAccessManager* m_networkManager;

    const QString apiLoginUrl = "http://localhost:18080/api/login";
    const QString apiRegisterUrl = "http://localhost:18080/api/register";
    const QString logoPath = ":/LOGO.png";
};
#endif