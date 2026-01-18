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

    Ui::MainMenu* ui;
    QString m_username;
    int m_userId;
    int m_gamePageIndex;

    NetworkManager* m_network;
    ProfileWidget* m_profileWidget;

    const int m_roomCodeLength = 4;
};

#endif