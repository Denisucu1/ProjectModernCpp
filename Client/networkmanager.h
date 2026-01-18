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
    void sendChatMessage(const QString& text);
    void sendGameAction(int cardValue, int stackIndex);

signals:
    void roomCreated(const QString& roomCode);
    void roomJoined(const QString& roomCode);
    void roomClosed(const QString& reason);
    void playerListUpdated(const QJsonArray& players);
    void chatMessageReceived(int senderId, const QString& text);
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

    const QString m_keyType = "type";
    const QString m_keyUserId = "userId";
    const QString m_keyMessage = "message";
    const QString m_keyRoomCode = "roomCode";
    const QString m_keySenderId = "senderId";
    const QString m_keyText = "text";
    const QString m_keyReason = "reason";
    const QString m_keyPlayers = "players";
    const QString m_keyStatus = "status";

    const QString m_typeChat = "chat";
    const QString m_typeCreateRoom = "create_room";
    const QString m_typeJoinRoom = "join_room";
    const QString m_typeStartGame = "start_game";
    const QString m_typeLeaveRoom = "leave_room";
    const QString m_typeRoomClosed = "room_closed";

    const QString m_statusRoomCreated = "room_created";
    const QString m_statusJoinedRoom = "joined_room";

    const QString m_profileEndpoint = "/api/profile/%1";

    const int m_endTurnValue = 0;
    const int m_victoryStatus = 1;
};

#endif