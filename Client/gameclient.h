#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QUrl>

class GameClient : public QObject
{
    Q_OBJECT
public:
    explicit GameClient(const QUrl& url, int userId, QObject* parent = nullptr);
    void connectToServer();
    void sendMessage(const QString& message);

signals:
    void messageReceived(const QString& message);
    void connected();
    void disconnected();

private slots:
    void onConnected();
    void onTextMessageReceived(const QString& message);
    void onDisconnected();

private:
    QWebSocket m_webSocket;
    QUrl m_url;
    int m_userId;
};

#endif