#include "gameclient.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

GameClient::GameClient(const QUrl& url, int userId, QObject* parent)
    : QObject(parent), m_url(url), m_userId(userId)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &GameClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &GameClient::onDisconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &GameClient::onTextMessageReceived);
}

void GameClient::connectToServer()
{
    m_webSocket.open(m_url);
}

void GameClient::sendMessage(const QString& message)
{
    if (m_webSocket.isValid()) {
        m_webSocket.sendTextMessage(message);
    }
}

void GameClient::onConnected()
{
    qDebug() << "WebSocket connected";
    emit connected();

    QJsonObject json;
    json["type"] = "login";
    json["userId"] = m_userId;

    QJsonDocument doc(json);
    sendMessage(doc.toJson(QJsonDocument::Compact));
}

void GameClient::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
    emit disconnected();
}

void GameClient::onTextMessageReceived(const QString& message)
{
    qDebug() << "Message received:" << message;
    emit messageReceived(message);
}