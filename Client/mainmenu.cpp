#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include <QApplication>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

MainMenu::MainMenu(const QString& username, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainMenu),
    m_username(username)
{
    ui->setupUi(this);
    m_networkManager = new QNetworkAccessManager(this);
    ui->stackedWidget->setCurrentIndex(0);
}

MainMenu::~MainMenu()
{
    delete ui;
}

void MainMenu::on_playButton_clicked()
{
    GameWindow* game = new GameWindow(this);
    game->show();
    this->close();
}

void MainMenu::on_exitButton_clicked()
{
    QApplication::quit();
}

void MainMenu::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainMenu::on_profileButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

    QJsonObject jsonRequest;
    jsonRequest["username"] = m_username;
    QJsonDocument jsonDoc(jsonRequest);
    QByteArray jsonData = jsonDoc.toJson();

    QUrl profileUrl("http://localhost:18080/api/profile");
    QNetworkRequest request(profileUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onProfileReply(reply);
        });

    ui->gamesPlayedLabel->setText("Loading...");
}

void MainMenu::onProfileReply(QNetworkReply* reply)
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

    }
    else {
        QString errorMsg = jsonObj["message"].toString();
        QMessageBox::warning(this, "Eroare Profil", "Nu am putut incarca profilul: " + errorMsg);
        ui->gamesPlayedLabel->setText("Eroare");
    }

    reply->deleteLater();
}