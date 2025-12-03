#include "gameclient.h"

GameClient::GameClient(const QUrl& url, int userId, QObject* parent)
    : QObject(parent)
    , m_url(url)
    , m_userId(userId)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &GameClient::onConnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &GameClient::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &GameClient::onClosed);
}

void GameClient::connectToServer()
{
    qDebug() << "Conectare la server:" << m_url;
    m_webSocket.open(m_url);
}

void GameClient::onConnected()
{
    qDebug() << "Conectat! Trimit login...";
    QJsonObject loginObj;
    loginObj["type"] = "login";
    loginObj["userId"] = m_userId;

    QJsonDocument doc(loginObj);
    m_webSocket.sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

void GameClient::onTextMessageReceived(QString message)
{
    qDebug() << "Mesaj primit:" << message;
}

void GameClient::onClosed()
{
    qDebug() << "Deconectat.";
}