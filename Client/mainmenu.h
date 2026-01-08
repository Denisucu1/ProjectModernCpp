#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class GameClient;
namespace Ui { class MainMenu; }

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(const QString& username, int userId, QWidget* parent = nullptr);
    ~MainMenu();

private slots:
    void on_playButton_clicked();
    void on_createGameButton_clicked();
    void on_joinGameButton_clicked();
    void on_confirmJoinButton_clicked();
    void on_cancelJoinButton_clicked();
    void on_exitButton_clicked();
    void on_backButton_clicked();
    void on_lobbyBackButton_clicked();
    void on_modeBackButton_clicked();
    void on_startGameButton_clicked();
    void on_closeLobbyButton_clicked();
    void on_profileButton_clicked();

    void onSocketMessage(const QString& message);
    void onGameBinaryMessage(const QByteArray& data);
    void onProfileReply(QNetworkReply* reply);

private:
    Ui::MainMenu* ui;
    QString m_username;
    int m_userId;
    QNetworkAccessManager* m_networkManager;
    GameClient* m_gameClient;
    int m_gamePageIndex;
};

#endif // MAINMENU_H