#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include <QApplication>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonArray>
#include <QListWidgetItem>

MainMenu::MainMenu(const QString& username, int userId, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainMenu),
    m_username(username),
    m_userId(userId)
{
    ui->setupUi(this);

    QString styleSheet = R"(
        QWidget#MainMenu, QWidget#lobbyPage, QWidget#profilePage, QWidget#modeSelectionPage {
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
        QPushButton#playButton, 
        QPushButton#createGameButton, 
        QPushButton#joinGameButton,
        QPushButton#confirmJoinButton,
        QPushButton#startGameButton { 
            background-color: #4CAF50;
        }
        QPushButton#playButton:hover, 
        QPushButton#createGameButton:hover, 
        QPushButton#joinGameButton:hover,
        QPushButton#confirmJoinButton:hover,
        QPushButton#startGameButton:hover {
            background-color: #45a049;
        }
        QPushButton#exitButton,
        QPushButton#modeBackButton,
        QPushButton#cancelJoinButton,
        QPushButton#closeLobbyButton {
            background-color: #F44336;
        }
        QPushButton#exitButton:hover,
        QPushButton#modeBackButton:hover,
        QPushButton#cancelJoinButton:hover,
        QPushButton#closeLobbyButton:hover {
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
            background-color: #E53935;
            border-top-left-radius: 15px;
            border-top-right-radius: 15px;
            max-width: 400px;
            padding: 10px;
        }
        QLineEdit#roomCodeInput {
            background-color: #4a4a4a; 
            color: white; 
            border: 2px solid #4CAF50; 
            border-radius: 10px; 
            padding: 10px; 
            font-size: 24px; 
            font-weight: bold; 
            min-height: 40px;
            text-transform: uppercase;
        }
        QLabel#generatedCodeLabel {
             color: #4CAF50;
             font-size: 48px;
             font-weight: bold;
             qproperty-alignment: 'AlignHCenter | AlignVCenter';
             border: 2px dashed #4CAF50;
             border-radius: 10px;
             padding: 10px;
             background-color: rgba(76, 175, 80, 0.1);
        }
        QLabel { color: white; }
    )";

    this->setStyleSheet(styleSheet);

    m_networkManager = new QNetworkAccessManager(this);
    GameWindow* gamePage = new GameWindow(this);
    ui->stackedWidget->addWidget(gamePage);
    ui->stackedWidget->setCurrentIndex(0);
    ui->usernameLabel->setText(m_username);

    QUrl wsUrl("ws://localhost:18080/ws/game");
    m_gameClient = new GameClient(wsUrl, m_userId, this);

    connect(m_gameClient, &GameClient::messageReceived, this, &MainMenu::onSocketMessage);

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

void MainMenu::on_createGameButton_clicked()
{
    QJsonObject json;
    json["type"] = "create_room";
    json["userId"] = m_userId;

    QJsonDocument doc(json);
    m_gameClient->sendMessage(doc.toJson(QJsonDocument::Compact));

    ui->createGameButton->setText("Creating...");
    ui->createGameButton->setEnabled(false);
}

void MainMenu::on_joinGameButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->roomCodeInput->clear();
    ui->roomCodeInput->setFocus();
}

void MainMenu::on_modeBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainMenu::on_confirmJoinButton_clicked()
{
    QString code = ui->roomCodeInput->text().toUpper().trimmed();
    if (code.length() != 4) {
        QMessageBox::warning(this, "Input Error", "Room code must be 4 characters.");
        return;
    }

    QJsonObject json;
    json["type"] = "join_room";
    json["userId"] = m_userId;
    json["roomCode"] = code;

    QJsonDocument doc(json);
    m_gameClient->sendMessage(doc.toJson(QJsonDocument::Compact));

    ui->confirmJoinButton->setText("Joining...");
    ui->confirmJoinButton->setEnabled(false);
}

void MainMenu::on_cancelJoinButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainMenu::onSocketMessage(const QString& message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isNull() || !doc.isObject()) return;

    QJsonObject json = doc.object();

    ui->createGameButton->setEnabled(true);
    ui->createGameButton->setText("Create Game");
    ui->confirmJoinButton->setEnabled(true);
    ui->confirmJoinButton->setText("Join Room");

    QString type = json["type"].toString();

    auto updatePlayerList = [&](const QJsonArray& playersArray) {
        ui->lobbyPlayersList->clear();

        for (const QJsonValue& value : playersArray) {
            QJsonObject playerObj = value.toObject();

            int playerId = playerObj["userId"].toInt();
            QString playerName = playerObj["username"].toString();

            if (playerName.isEmpty()) {
                playerName = QString("Jucător %1").arg(playerId);
            }

            QString itemText = QString("%1 (ID: %2)").arg(playerName).arg(playerId);

            if (playerId == m_userId) {
                itemText += " (Tu)";
            }

            QListWidgetItem* item = new QListWidgetItem(itemText);
            ui->lobbyPlayersList->addItem(item);
        }
        };

    if (type == "room_update") {
        if (json.contains("players") && json["players"].isArray()) {
            updatePlayerList(json["players"].toArray());
        }
        return;
    }

    if (json.contains("status")) {
        QString status = json["status"].toString();

        if (status == "room_created") {
            QString code = json["roomCode"].toString();
            ui->generatedCodeLabel->setText(code);
            ui->stackedWidget->setCurrentIndex(4);

            ui->lobbyPlayersList->clear();
            ui->lobbyPlayersList->addItem(new QListWidgetItem(QString("%1 (ID: %2) (Tu)").arg(m_username).arg(m_userId)));
        }
        else if (status == "joined_room") {
            QString code = json["roomCode"].toString();
            ui->generatedCodeLabel->setText(code);
            ui->stackedWidget->setCurrentIndex(4);

            if (json.contains("players") && json["players"].isArray()) {
                updatePlayerList(json["players"].toArray());
            }
        }
        else if (status == "error") {
            QString msg = json["message"].toString();
            QMessageBox::warning(this, "Eroare", msg);
        }
    }
}
void MainMenu::on_startGameButton_clicked()
{
    QJsonObject json;
    json["type"] = "start_game";
    json["userId"] = m_userId;
    json["roomCode"] = ui->generatedCodeLabel->text();

    QJsonDocument doc(json);
    m_gameClient->sendMessage(doc.toJson(QJsonDocument::Compact));
}

void MainMenu::on_closeLobbyButton_clicked()
{
    QJsonObject json;
    json["type"] = "leave_room";
    json["userId"] = m_userId;

    QJsonDocument doc(json);
    m_gameClient->sendMessage(doc.toJson(QJsonDocument::Compact));

    ui->stackedWidget->setCurrentIndex(2);
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
        ui->gamesPlayedLabel->setText("Eroare");
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    if (jsonObj.contains("error")) {
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