#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QJsonArray>
#include "networkmanager.h"
#include "profilewidget.h"
#include "GameProtocol.pb.h"

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

    void HandleRoomCreated(const QString& code);
    void HandleRoomJoined(const QString& code);
    void HandlePlayerListUpdate(const QJsonArray& players);
    void HandleGameStateUpdate(const myproject::GameState& state);
    void HandleGameOver(bool isVictory);
    void HandleProfileData(const QJsonObject& data);

private:
    void InitializeUi();
    void ConnectUiSignals();
    void SetupNavigationLayout();
    void SetupMenuPages();
    void SetupProfilePage();
    void SetupLobbyPages();
    void SetupNetwork();

    void SwitchToPage(PageIndex page);

    Ui::MainMenu* ui;
    QString m_username;
    int m_userId;
    int m_gamePageIndex;

    NetworkManager* m_network; 
    ProfileWidget* m_profileWidget;

    const int RoomCodeLength = 4;
};

#endif