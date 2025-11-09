#include "profilewindow.h"
#include "ui_profilewindow.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

ProfileWindow::ProfileWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfileWindow)
{
    ui->setupUi(this);
    m_networkManager = new QNetworkAccessManager(this);
}

ProfileWindow::~ProfileWindow()
{
    delete ui;
}

void ProfileWindow::fetchProfileData(const QString &username)
{
    QJsonObject jsonRequest;
    jsonRequest["username"] = username;
    QJsonDocument jsonDoc(jsonRequest);
    QByteArray jsonData = jsonDoc.toJson();

    QUrl profileUrl("http://localhost:18080/api/profile");
    QNetworkRequest request(profileUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onProfileReply(reply);
    });

    ui->gamesPlayedLabel->setText("Loading...");
}

void ProfileWindow::onProfileReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Eroare Retea", "Eroare de conexiune: " + reply->errorString());
        ui->gamesPlayedLabel->setText("Eroare");
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj["success"].toBool()) {
        QJsonObject profileData = jsonObj["data"].toObject();

        ui->gamesPlayedLabel->setText(QString::number(profileData["games_played"].toInt()));
        ui->gamesWonLabel->setText(QString::number(profileData["games_won"].toInt()));
        ui->cardsAtLossLabel->setText(QString::number(profileData["total_cards_at_loss"].toInt()));
        ui->totalTimeLabel->setText(QString::number(profileData["total_time_minutes"].toInt()));
        ui->performanceScoreLabel->setText(QString::number(profileData["performance_score"].toDouble()));

    } else {
        QString errorMsg = jsonObj["message"].toString();
        QMessageBox::warning(this, "Eroare Profil", "Nu am putut incarca profilul: " + errorMsg);
        ui->gamesPlayedLabel->setText("Eroare");
    }

    reply->deleteLater();
}
