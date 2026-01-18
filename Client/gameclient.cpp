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

    connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &GameClient::onBinaryMessageReceived);
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

    QJsonObject login;
    login[m_keyType] = m_valueLogin;
    login[m_keyUserId] = m_userId;
    m_webSocket.sendTextMessage(QJsonDocument(login).toJson(QJsonDocument::Compact));

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

void GameClient::sendBinaryMessage(const QByteArray& data) {
    if (m_webSocket.isValid()) {
        m_webSocket.sendBinaryMessage(data);
    }
}

void GameClient::onBinaryMessageReceived(const QByteArray& message) {
    emit binaryMessageReceived(message);
}