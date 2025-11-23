#pragma once
#include <QMainWindow>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_registerButton_clicked();
    void on_loginButton_clicked();
    void onLoginReply(QNetworkReply* reply);
    void onRegisterReply(QNetworkReply* reply);

private:
    Ui::MainWindow* ui;
    QNetworkAccessManager* m_networkManager;
};