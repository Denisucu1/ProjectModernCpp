#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "gameclient.h"
#include "GameProtocol.pb.h"

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(const QString& wsUrl, const QString& apiBaseUrl, int userId, QObject* parent = nullptr);

    void connectToServer();

    void createRoom();
    void joinRoom(const QString& roomCode);
    void startGame(const QString& roomCode);
    void leaveRoom();

    void fetchProfileStats();

    void sendGameAction(int cardValue, int stackIndex);

signals:
    void roomCreated(const QString& roomCode);
    void roomJoined(const QString& roomCode);
    void roomClosed(const QString& reason);
    void playerListUpdated(const QJsonArray& players);

    void gameStarted();
    void gameStateUpdated(const myproject::GameState& state);
    void gameOver(bool isVictory);

    void profileStatsReceived(const QJsonObject& stats);
    void errorOccurred(const QString& errorMsg);

private slots:
    void onSocketMessageReceived(const QString& message);
    void onBinaryMessageReceived(const QByteArray& data);
    void onProfileReplyFinished();

private:
    void processJsonResponse(const QJsonObject& json);

    int m_userId;
    QString m_wsUrl;
    QString m_apiBaseUrl;

    GameClient* m_gameClient;
    QNetworkAccessManager* m_restManager;
};

#endif 