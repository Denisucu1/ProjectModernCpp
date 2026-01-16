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
    j["type"] = "chat";
    j["userId"] = m_userId; 
    j["message"] = text;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::createRoom() {
    QJsonObject j;
    j["type"] = "create_room";
    j["userId"] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::joinRoom(const QString& roomCode) {
    QJsonObject j;
    j["type"] = "join_room";
    j["userId"] = m_userId;
    j["roomCode"] = roomCode;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::startGame(const QString& roomCode) {
    QJsonObject j;
    j["type"] = "start_game";
    j["userId"] = m_userId;
    j["roomCode"] = roomCode;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::leaveRoom() {
    QJsonObject j;
    j["type"] = "leave_room";
    j["userId"] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}

void NetworkManager::fetchProfileStats() {
    QUrl url(m_apiBaseUrl + QString("/api/profile/%1").arg(m_userId));
    QNetworkRequest request(url);
    QNetworkReply* reply = m_restManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &NetworkManager::onProfileReplyFinished);
}

void NetworkManager::sendGameAction(int cardValue, int stackIndex) {
    myproject::GameMessage msg;
    msg.set_type(myproject::GameMessage::ACTION);
    auto* action = msg.mutable_action();

    if (cardValue == 0) {
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

    if (json["type"].toString() == "chat") {
        int senderId = json["senderId"].toInt();
        QString text = json["text"].toString(); 
        emit chatMessageReceived(senderId, text);
        return;
    }

    if (json["type"].toString() == "room_closed") {
        emit roomClosed(json["reason"].toString());
        return;
    }

    if (json.contains("players")) {
        emit playerListUpdated(json["players"].toArray());
    }

    if (json.contains("status")) {
        processJsonResponse(json);
    }
}

void NetworkManager::processJsonResponse(const QJsonObject& json) {
    QString status = json["status"].toString();
    if (status == "room_created") {
        emit roomCreated(json["roomCode"].toString());
    }
    else if (status == "joined_room") {
        emit roomJoined(json["roomCode"].toString());
    }
}

void NetworkManager::onBinaryMessageReceived(const QByteArray& data) {
    myproject::GameMessage msg;
    if (!msg.ParseFromArray(data.constData(), data.size())) return;

    if (msg.type() == myproject::GameMessage_Type_GAME_OVER) {
        emit gameOver(msg.state().game_status() == 1);
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