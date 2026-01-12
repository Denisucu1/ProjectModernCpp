#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "gameclient.h"

namespace Ui { class MainMenu; }

class MainMenu : public QWidget {
    Q_OBJECT
public:
    explicit MainMenu(const QString& username, int userId, QWidget* parent = nullptr);
    ~MainMenu();
private slots:
    void on_playButton_clicked();
    void on_exitButton_clicked();
    void on_backButton_clicked();
    void on_lobbyBackButton_clicked();
    void on_modeBackButton_clicked();
    void on_cancelJoinButton_clicked();
    void on_createGameButton_clicked();
    void on_joinGameButton_clicked();
    void on_confirmJoinButton_clicked();
    void on_startGameButton_clicked();
    void on_closeLobbyButton_clicked();
    void on_profileButton_clicked();
    void onProfileReply(QNetworkReply* reply);
    void onSocketMessage(const QString& message);
    void onGameBinaryMessage(const QByteArray& data);
private:
    Ui::MainMenu* ui;
    QString m_username;
    int m_userId;
    int m_gamePageIndex;
    GameClient* m_gameClient;
    QNetworkAccessManager* m_networkManager;
};
#endif