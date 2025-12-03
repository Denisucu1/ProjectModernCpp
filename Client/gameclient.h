#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrl>
#include <QDebug>

class GameClient : public QObject
{
    Q_OBJECT
public:
    explicit GameClient(const QUrl& url, int userId = 123, QObject* parent = nullptr);
    void connectToServer();

private slots:
    void onConnected();
    void onTextMessageReceived(QString message);
    void onClosed();

private:
    QWebSocket m_webSocket;
    QUrl m_url;
    int m_userId;
};

#endif