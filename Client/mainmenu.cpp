#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include "profilewindow.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

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
    GameWindow* game = new GameWindow();
    game->show();
    this->close();
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
}

void MainMenu::onProfileReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Eroare", "Nu s-a putut incarca profilul.");
        reply->deleteLater();
        return;
    }
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj["success"].toBool()) {
        QJsonObject data = jsonObj["data"].toObject();
        ui->gamesPlayedLabel->setText(QString::number(data["games_played"].toInt()));
        ui->gamesWonLabel->setText(QString::number(data["games_won"].toInt()));
        ui->cardsAtLossLabel->setText(QString::number(data["total_cards_at_loss"].toInt()));
        ui->totalTimeLabel->setText(QString::number(data["total_time_minutes"].toInt()));
        ui->performanceScoreLabel->setText(QString::number(data["performance_score"].toDouble()));
    }

    reply->deleteLater();
}

void MainMenu::on_backButton_clicked() 
{
    if (this->parentWidget()) {
        this->parentWidget()->show();
    }
    this->close();
}

void MainMenu::on_exitButton_clicked()
{
    QApplication::quit();
}