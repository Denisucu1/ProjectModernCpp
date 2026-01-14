#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include "gameclient.h"
#include "GameProtocol.pb.h"

namespace Ui { class MainMenu; }

class MainMenu : public QWidget {
    Q_OBJECT

public:
    explicit MainMenu(const QString& username, int userId, QWidget* parent = nullptr);
    ~MainMenu() override;

private slots:
    void on_playButton_clicked();
    void on_exitButton_clicked();
    void on_backButton_clicked();
    void on_modeBackButton_clicked();
    void on_cancelJoinButton_clicked();
    void on_createGameButton_clicked();
    void on_joinGameButton_clicked();
    void on_confirmJoinButton_clicked();
    void on_startGameButton_clicked();
    void on_profileButton_clicked();
    void on_closeLobbyButton_clicked();
    void onSocketMessage(const QString& message);
    void onGameBinaryMessage(const QByteArray& data);
    void onProfileReply(QNetworkReply* reply);

private:
    void applyStyles();
    void setupNavigationLayout();
    void setupMenuPages();
    void setupProfilePage();
    void setupLobbyPages();
    void setupGameLogic();
    void updatePlayerList(const QJsonArray& players);
    void processRoomStatus(const QJsonObject& json);
    void updateGameInterface(const myproject::GameState& state);

    Ui::MainMenu* ui;
    QString m_username;
    int m_userId;
    int m_gamePageIndex;
    GameClient* m_gameClient;
    QNetworkAccessManager* m_networkManager;

    const int roomCodeInputWidth = 300;
    const int profileCardWidth = 450;
    const int roomCodeLength = 4;
    const int menuPageIndex = 0;
    const int profilePageIndex = 1;
    const int modeSelectionPageIndex = 2;
    const int joinRoomPageIndex = 3;
    const int lobbyWaitPageIndex = 4;
};

#endif