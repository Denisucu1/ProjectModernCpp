#pragma once
#include <QWidget>
#include <QNetworkAccessManager>
#include <QString>

namespace Ui {
    class ProfileWindow;
}

class ProfileWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileWindow(QWidget* parent = nullptr);
    ~ProfileWindow();
    void fetchProfileData(const QString& username);

private slots:
    void onProfileReply(QNetworkReply* reply);

private:
    Ui::ProfileWindow* ui;
    QNetworkAccessManager* m_networkManager;
};