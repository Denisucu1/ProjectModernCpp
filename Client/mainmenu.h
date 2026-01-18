#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QJsonArray>
#include <QString>
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
    void onPlayButtonClicked();
    void onExitButtonClicked();
    void onBackButtonClicked();
    void onModeBackButtonClicked();
    void onCancelJoinButtonClicked();
    void onCreateGameButtonClicked();
    void onJoinGameButtonClicked();
    void onConfirmJoinButtonClicked();
    void onStartGameButtonClicked();
    void onProfileButtonClicked();
    void onCloseLobbyButtonClicked();

    void handleRoomCreated(const QString& code);
    void handleRoomJoined(const QString& code);
    void handlePlayerListUpdate(const QJsonArray& players);
    void handleGameStateUpdate(const myproject::GameState& state);
    void handleGameOver(bool isVictory);
    void handleProfileData(const QJsonObject& data);

private:
    void initializeUi();
    void connectUiSignals();
    void setupNavigationLayout();
    void setupMenuPages();
    void setupProfilePage();
    void setupLobbyPages();
    void setupNetwork();
    void switchToPage(PageIndex page);

    Ui::MainMenu* m_ui;
    QString m_username;
    int m_userId;
    int m_gamePageIndex;

    NetworkManager* m_network;
    ProfileWidget* m_profileWidget;

    const int m_roomCodeLength = 4;
    const int m_minWinWidth = 850;
    const int m_maxWinWidth = 16777215;
    const int m_marginZero = 0;
    const int m_marginStandard = 20;
    const int m_spacingStandard = 20;
    const int m_spacingLarge = 30;
    const int m_stretchFactor = 1;

    const QString m_wsUrl = "ws://localhost:18080/ws/game";
    const QString m_apiUrl = "http://localhost:18080";

    const QString m_keyUsername = "username";
    const QString m_keyUserId = "userId";

    const QString m_playerDisplayFormat = "%1 (ID: %2)";

    const QString m_titleSessionClosed = "Closed Session";
    const QString m_titleGameOver = "Game Over";
    const QString m_msgVictory = "Victory!";
    const QString m_msgDefeat = "Defeat!";
};

#endif