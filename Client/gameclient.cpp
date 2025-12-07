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
    qDebug() << "WebSocket conectat! Trimit datele de autentificare...";

    QJsonObject loginObj;
    loginObj["type"] = "login";
    loginObj["userId"] = m_userId;

    QJsonDocument doc(loginObj);
    QString message = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    m_webSocket.sendTextMessage(message);
}

void GameClient::onTextMessageReceived(QString message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("status")) {
            qDebug() << "STATUS PRIMIT DE LA SERVER:" << obj["status"].toString();
        }
    }
}

void GameClient::onClosed()
{
    qDebug() << "Conexiunea WebSocket s-a inchis.";
}