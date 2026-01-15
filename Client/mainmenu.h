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
    void OnPlayButtonClicked();
    void OnExitButtonClicked();
    void OnBackButtonClicked();
    void OnModeBackButtonClicked();
    void OnCancelJoinButtonClicked();
    void OnCreateGameButtonClicked();
    void OnJoinGameButtonClicked();
    void OnConfirmJoinButtonClicked();
    void OnStartGameButtonClicked();
    void OnProfileButtonClicked();
    void OnCloseLobbyButtonClicked();
    void OnSocketMessage(const QString& message);
    void OnGameBinaryMessage(const QByteArray& data);
    void OnProfileReply(QNetworkReply* reply);

private:
    void InitializeUi();
    void ConnectUiSignals();
    void SetupNavigationLayout();
    void SetupMenuPages();
    void SetupProfilePage();
    void SetupLobbyPages();
    void SetupGameLogic();

    void UpdatePlayerList(const QJsonArray& players);
    void ProcessRoomStatus(const QJsonObject& json);
    void UpdateGameInterface(const myproject::GameState& state);
    void SwitchToPage(PageIndex page);

    Ui::MainMenu* ui;
    QString m_username;
    int m_userId;
    int m_gamePageIndex;
    GameClient* m_gameClient;
    QNetworkAccessManager* m_networkManager;
    ProfileWidget* m_profileWidget;

    const int RoomCodeLength = 4;
    const QString ServerWsUrl = "ws://localhost:18080/ws/game";
    const QString ProfileUrlTemplate = "http://localhost:18080/api/profile/%1";
};

#endif