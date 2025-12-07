#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include <QApplication>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

MainMenu::MainMenu(const QString& username, int userId, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainMenu),
    m_username(username),
    m_userId(userId)
{
    ui->setupUi(this);

    QString styleSheet = R"(
        
        QWidget#MainMenu, QWidget#lobbyPage, QWidget#profilePage {
            background-color: #1e1e1e;
        }

        QPushButton {
            background-color: #4a4a4a;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px;
            font-size: 14px;
            font-weight: bold;
            min-height: 30px;
            margin-top: 5px;
        }

        QPushButton:hover {
            background-color: #616161;
        }

        QPushButton#playButton {
            background-color: #4CAF50;
        }
        QPushButton#playButton:hover {
            background-color: #45a049;
        }

        QPushButton#exitButton {
            background-color: #F44336;
        }
        QPushButton#exitButton:hover {
            background-color: #E53935;
        }

        QPushButton#profileButton {
            background-color: #2196F3;
        }
        QPushButton#profileButton:hover {
            background-color: #1E88E5;
        }

        QPushButton#backButton, QPushButton#lobbyBackButton {
            background-color: #F44336;
            max-width: 400px;
        }
        QPushButton#backButton:hover, QPushButton#lobbyBackButton:hover {
            background-color: #E53935;
        }

        QLabel#usernameLabel {
            font-size: 24px;
            font-weight: bold;
            color: white;
            qproperty-alignment: 'AlignHCenter | AlignVCenter';
            margin-bottom: 0px;
            padding: 10px;
            background-color: #E53935;
            border-top-left-radius: 15px;
            border-top-right-radius: 15px;
            max-width: 400px;
        }

        QGridLayout {
            background-color: #E53935;
            margin: 0px;
            padding: 10px;
            border-bottom-left-radius: 15px;
            border-bottom-right-radius: 15px;
            max-width: 400px;
        }

        QGridLayout QLabel {
            color: white;
            font-size: 14px;
            font-weight: bold;
            background-color: rgba(0, 0, 0, 0.2);
            padding: 8px;
            margin: 2px;
            border-radius: 4px;
        }

        QLabel#gamesPlayedLabel,
        QLabel#gamesWonLabel,
        QLabel#cardsAtLossLabel,
        QLabel#totalTimeLabel,
        QLabel#performanceScoreLabel {
            qproperty-alignment: 'AlignVCenter | AlignRight';
            color: #f5f5f5;
            font-size: 16px;
        }
        
    )";

    this->setStyleSheet(styleSheet);

    m_networkManager = new QNetworkAccessManager(this);
    GameWindow* gamePage = new GameWindow(this);
    ui->stackedWidget->addWidget(gamePage);
    ui->stackedWidget->setCurrentIndex(0);
    ui->usernameLabel->setText(m_username);

    QUrl wsUrl("ws://localhost:18080/ws/game");
    m_gameClient = new GameClient(wsUrl, m_userId, this);
    m_gameClient->connectToServer();
}

MainMenu::~MainMenu()
{
    delete ui;
}

void MainMenu::on_playButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainMenu::on_exitButton_clicked()
{
    QApplication::quit();
}

void MainMenu::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainMenu::on_lobbyBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainMenu::on_profileButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    QString urlString = QString("http://localhost:18080/api/profile/%1").arg(m_userId);
    QUrl profileUrl(urlString);

    QNetworkRequest request(profileUrl);
    QNetworkReply* reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onProfileReply(reply);
        });

    ui->gamesPlayedLabel->setText("Loading...");
}

void MainMenu::onProfileReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QString errStr = reply->errorString();
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        if (!jsonDoc.isNull() && jsonDoc.object().contains("error")) {
            errStr = jsonDoc.object()["error"].toString();
        }

        QMessageBox::critical(this, "Eroare Profil", "Nu am putut incarca profilul: " + errStr);
        ui->gamesPlayedLabel->setText("Eroare");
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj.contains("error")) {
        QString errorMsg = jsonObj["error"].toString();
        QMessageBox::warning(this, "Eroare Profil", "Server: " + errorMsg);
        ui->gamesPlayedLabel->setText("Eroare");
    }
    else {
        ui->gamesPlayedLabel->setText(QString::number(jsonObj["gamesPlayed"].toInt()));
        ui->gamesWonLabel->setText(QString::number(jsonObj["gamesWon"].toInt()));
        ui->cardsAtLossLabel->setText(QString::number(jsonObj["cardsLeftOnLosses"].toInt()));

        ui->totalTimeLabel->setText(QString::number(jsonObj["hoursPlayed"].toInt()));

        ui->performanceScoreLabel->setText(QString::number(jsonObj["performanceScore"].toDouble(), 'f', 2)); 
    }

    reply->deleteLater();
}