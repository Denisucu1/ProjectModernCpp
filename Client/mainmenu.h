#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include "gameclient.h"
#include "GameProtocol.pb.h"
#include "profilewidget.h"

namespace Ui { class MainMenu; }

class MainMenu : public QWidget {
    Q_OBJECT

public:
    enum class PageIndex {
        menu = 0,
        profile = 1,
        modeSelection = 2,
        joinRoom = 3,
        lobbyWait = 4,
        game = 5
    };

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
    void initializeUi();
    void setupNavigationLayout();
    void setupMenuPages();
    void setupProfilePage();
    void setupLobbyPages();
    void setupGameLogic();

    void updatePlayerList(const QJsonArray& players);
    void processRoomStatus(const QJsonObject& json);
    void updateGameInterface(const myproject::GameState& state);
    void switchToPage(PageIndex page);

    Ui::MainMenu* ui;
    QString m_username;
    int m_userId;
    int m_gamePageIndex;
    GameClient* m_gameClient;
    QNetworkAccessManager* m_networkManager;
    ProfileWidget* m_profileWidget;

    const int m_roomCodeLength = 4;
    const QString m_wsUrl = "ws://localhost:18080/ws/game";
    const QString m_profileUrlTemplate = "http://localhost:18080/api/profile/%1";
};

#endif