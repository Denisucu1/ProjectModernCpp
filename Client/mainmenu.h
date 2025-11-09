#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
    class MainMenu;
}

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(const QString& username, QWidget* parent = nullptr);
    ~MainMenu();

private slots:
    void on_playButton_clicked();
    void on_profileButton_clicked();
    void on_exitButton_clicked();
    void on_backButton_clicked();
    void on_lobbyBackButton_clicked();
    void onProfileReply(QNetworkReply* reply);

private:
    Ui::MainMenu* ui;
    QString m_username;
    QNetworkAccessManager* m_networkManager;
};

#endif