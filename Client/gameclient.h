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
    void sendBinaryMessage(const QByteArray& data);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString& message);
    void binaryMessageReceived(const QByteArray& message);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onBinaryMessageReceived(const QByteArray& message);

private:
    QWebSocket m_webSocket;
    QUrl m_url;
    int m_userId;
};

#endif