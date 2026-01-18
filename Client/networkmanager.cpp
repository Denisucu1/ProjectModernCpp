#include "networkmanager.h"
#include <QJsonDocument>

NetworkManager::NetworkManager(const QString& wsUrl, const QString& apiBaseUrl, int userId, QObject* parent)
    : QObject(parent), m_userId(userId), m_wsUrl(wsUrl), m_apiBaseUrl(apiBaseUrl) {

    m_restManager = new QNetworkAccessManager(this);
    m_gameClient = new GameClient(QUrl(m_wsUrl), m_userId, this);

    connect(m_gameClient, &GameClient::messageReceived, this, &NetworkManager::onSocketMessageReceived);
    connect(m_gameClient, &GameClient::binaryMessageReceived, this, &NetworkManager::onBinaryMessageReceived);
}

void NetworkManager::connectToServer() {
    m_gameClient->connectToServer();
}

void NetworkManager::sendChatMessage(const QString& text) {
    if (text.trimmed().isEmpty()) return;

    QJsonObject j;
    j[m_keyType] = m_typeChat;
    j[m_keyUserId] = m_userId;
    j[m_keyMessage] = text;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::createRoom() {
    QJsonObject j;
    j[m_keyType] = m_typeCreateRoom;
    j[m_keyUserId] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::joinRoom(const QString& roomCode) {
    QJsonObject j;
    j[m_keyType] = m_typeJoinRoom;
    j[m_keyUserId] = m_userId;
    j[m_keyRoomCode] = roomCode;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::startGame(const QString& roomCode) {
    QJsonObject j;
    j[m_keyType] = m_typeStartGame;
    j[m_keyUserId] = m_userId;
    j[m_keyRoomCode] = roomCode;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::leaveRoom() {
    QJsonObject j;
    j[m_keyType] = m_typeLeaveRoom;
    j[m_keyUserId] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::fetchProfileStats() {
    QUrl url(m_apiBaseUrl + m_profileEndpoint.arg(m_userId));
    QNetworkRequest request(url);
    QNetworkReply* reply = m_restManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &NetworkManager::onProfileReplyFinished);
}

void NetworkManager::sendGameAction(int cardValue, int stackIndex) {
    myproject::GameMessage msg;
    msg.set_type(myproject::GameMessage::ACTION);
    auto* action = msg.mutable_action();

    if (cardValue == m_endTurnValue) {
        action->set_action_type(myproject::PlayerAction::END_TURN);
    }
    else {
        action->set_action_type(myproject::PlayerAction::PLAY_CARD);
        action->set_card_value(cardValue);
        action->set_stack_index(stackIndex);
    }

    m_gameClient->sendBinaryMessage(QByteArray::fromStdString(msg.SerializeAsString()));
}

void NetworkManager::onSocketMessageReceived(const QString& message) {
    QJsonObject json = QJsonDocument::fromJson(message.toUtf8()).object();
    QString typeValue = json[m_keyType].toString();

    if (typeValue == m_typeChat) {
        int senderId = json[m_keySenderId].toInt();
        QString text = json[m_keyText].toString();
        emit chatMessageReceived(senderId, text);
        return;
    }

    if (typeValue == m_typeRoomClosed) {
        emit roomClosed(json[m_keyReason].toString());
        return;
    }

    if (json.contains(m_keyPlayers)) {
        emit playerListUpdated(json[m_keyPlayers].toArray());
    }

    if (json.contains(m_keyStatus)) {
        processJsonResponse(json);
    }
}

void NetworkManager::processJsonResponse(const QJsonObject& json) {
    QString statusValue = json[m_keyStatus].toString();
    if (statusValue == m_statusRoomCreated) {
        emit roomCreated(json[m_keyRoomCode].toString());
    }
    else if (statusValue == m_statusJoinedRoom) {
        emit roomJoined(json[m_keyRoomCode].toString());
    }
}

void NetworkManager::onBinaryMessageReceived(const QByteArray& data) {
    myproject::GameMessage msg;
    if (!msg.ParseFromArray(data.constData(), data.size())) return;

    if (msg.type() == myproject::GameMessage_Type_GAME_OVER) {
        emit gameOver(msg.state().game_status() == m_victoryStatus);
    }
    else if (msg.type() == myproject::GameMessage_Type_STATE_UPDATE && msg.has_state()) {
        emit gameStateUpdated(msg.state());
    }
}

void NetworkManager::onProfileReplyFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        emit profileStatsReceived(json);
    }
    else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}