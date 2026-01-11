#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include "gameclient.h"
#include <QApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonArray>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QDebug>
#include "GameProtocol.pb.h"

MainMenu::MainMenu(const QString& username, int userId, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainMenu),
    m_username(username),
    m_userId(userId)
{
    ui->setupUi(this);

    for (int i = 0; i < ui->stackedWidget->count(); ++i) {
        QWidget* page = ui->stackedWidget->widget(i);
        
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(page->layout());
        if (!layout) {
            layout = new QVBoxLayout(page);
        }

        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(15); 
        layout->setAlignment(Qt::AlignCenter);

        layout->insertStretch(0, 1);     
        layout->addStretch(1);        
    }

    if (!this->layout()) {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addStretch(1);
        mainLayout->addWidget(ui->stackedWidget, 0, Qt::AlignCenter);
        mainLayout->addStretch(1);
        this->setLayout(mainLayout);
    }

    QString styleSheet = R"(
        QWidget#MainMenu { background-color: #1e1e1e; }
        
        QPushButton { 
            background-color: #4a4a4a; 
            color: white; 
            border-radius: 20px;   
            font-weight: bold; 
            font-size: 15px;
            
            min-width: 200px; 
            max-width: 200px; 
            min-height: 50px; 
            max-height: 50px;
            
            border: none;
        }
        
        QPushButton:hover { background-color: #5a5a5a; }
        
        QPushButton#playButton, QPushButton#createGameButton, 
        QPushButton#joinGameButton, QPushButton#confirmJoinButton, 
        QPushButton#startGameButton { background-color: #4CAF50; }
        
        QPushButton#exitButton, QPushButton#backButton, 
        QPushButton#cancelJoinButton, QPushButton#lobbyBackButton, 
        QPushButton#modeBackButton, QPushButton#closeLobbyButton { background-color: #D32F2F; }
        
        QLineEdit { 
            padding: 8px; 
            border-radius: 10px; 
            background-color: #2b2b2b; 
            color: white; 
            border: 1px solid #4CAF50; 
            min-width: 200px;
            max-width: 200px;
            min-height: 40px;
        }
        
        QListWidget { background-color: #2b2b2b; color: white; border-radius: 10px; border: 1px solid #4a4a4a; }
        QLabel { color: white; font-size: 14px; }
    )";
    this->setStyleSheet(styleSheet);

    m_networkManager = new QNetworkAccessManager(this);
    GameWindow* gamePage = new GameWindow(this);
    m_gamePageIndex = ui->stackedWidget->addWidget(gamePage);

    ui->stackedWidget->setCurrentIndex(0);
    ui->usernameLabel->setText(m_username);

    QUrl wsUrl("ws://localhost:18080/ws/game");
    m_gameClient = new GameClient(wsUrl, m_userId, this);

    connect(m_gameClient, &GameClient::connected, this, [this]() {
        QJsonObject login;
        login["type"] = "login"; login["userId"] = m_userId;
        m_gameClient->sendMessage(QJsonDocument(login).toJson(QJsonDocument::Compact));
    });

    connect(m_gameClient, &GameClient::messageReceived, this, &MainMenu::onSocketMessage);
    connect(m_gameClient, &GameClient::binaryMessageReceived, this, &MainMenu::onGameBinaryMessage);

    connect(gamePage, &GameWindow::playerMoved, this, [this](int cardValue, int stackIndex) {
        myproject::GameMessage msg;
        msg.set_type(myproject::GameMessage::ACTION);
        auto* action = msg.mutable_action();
        if (cardValue == 0) action->set_action_type(myproject::PlayerAction::END_TURN);
        else {
            action->set_action_type(myproject::PlayerAction::PLAY_CARD);
            action->set_card_value(cardValue);
            action->set_stack_index(stackIndex);
        }
        m_gameClient->sendBinaryMessage(QByteArray::fromStdString(msg.SerializeAsString()));
    });

    m_gameClient->connectToServer();
}

MainMenu::~MainMenu() { delete ui; }

void MainMenu::on_playButton_clicked() { ui->stackedWidget->setCurrentIndex(2); }
void MainMenu::on_exitButton_clicked() { QApplication::quit(); }
void MainMenu::on_backButton_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void MainMenu::on_lobbyBackButton_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void MainMenu::on_modeBackButton_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void MainMenu::on_cancelJoinButton_clicked() { ui->stackedWidget->setCurrentIndex(2); }

void MainMenu::on_createGameButton_clicked() {
    QJsonObject json; json["type"] = "create_room"; json["userId"] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
    ui->createGameButton->setEnabled(false);
}

void MainMenu::on_joinGameButton_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
    ui->roomCodeInput->clear(); 
}

void MainMenu::on_confirmJoinButton_clicked() {
    QString code = ui->roomCodeInput->text().toUpper().trimmed();
    if (code.length() != 4) return;
    QJsonObject json; json["type"] = "join_room"; json["userId"] = m_userId; json["roomCode"] = code;
    m_gameClient->sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void MainMenu::on_startGameButton_clicked() {
    QJsonObject json; 
    json["type"] = "start_game"; json["userId"] = m_userId; 
    json["roomCode"] = ui->generatedCodeLabel->text();
    m_gameClient->sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void MainMenu::on_closeLobbyButton_clicked() {
    QJsonObject json; json["type"] = "leave_room"; json["userId"] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
    ui->stackedWidget->setCurrentIndex(2);
}

void MainMenu::on_profileButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    QNetworkRequest req(QUrl(QString("http://localhost:18080/api/profile/%1").arg(m_userId)));
    QNetworkReply* reply = m_networkManager->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onProfileReply(reply); });
}

void MainMenu::onProfileReply(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        auto json = QJsonDocument::fromJson(reply->readAll()).object();
        ui->gamesPlayedLabel->setText(QString::number(json["gamesPlayed"].toInt()));
        ui->gamesWonLabel->setText(QString::number(json["gamesWon"].toInt()));
    }
    reply->deleteLater();
}

void MainMenu::onSocketMessage(const QString& message) {
    QJsonObject json = QJsonDocument::fromJson(message.toUtf8()).object();
    ui->createGameButton->setEnabled(true);
    if (json.contains("players")) {
        ui->lobbyPlayersList->clear();
        for (const auto& val : json["players"].toArray()) {
            QJsonObject p = val.toObject();
            ui->lobbyPlayersList->addItem(p["username"].toString() + " (ID: " + QString::number(p["userId"].toInt()) + ")");
        }
    }
    if (json.contains("status")) {
        QString status = json["status"].toString();
        if (status == "room_created" || status == "joined_room") {
            ui->generatedCodeLabel->setText(json["roomCode"].toString());
            ui->stackedWidget->setCurrentIndex(4);
        }
    }
}

void MainMenu::onGameBinaryMessage(const QByteArray& data) {
    if (ui->stackedWidget->currentIndex() == 4) ui->stackedWidget->setCurrentIndex(m_gamePageIndex);
    myproject::GameMessage msg;
    if (!msg.ParseFromArray(data.constData(), data.size())) return;
    if (msg.type() == myproject::GameMessage::STATE_UPDATE && msg.has_state()) {
        const auto& state = msg.state();
        std::vector<int> piles;
        for (int val : state.stack_tops()) piles.push_back(val);
        std::vector<int> hand;
        for (const auto& p : state.players()) {
            if (p.user_id() == m_userId) {
                for (int card : p.your_cards()) hand.push_back(card);
                break;
            }
        }
        if (GameWindow* gw = qobject_cast<GameWindow*>(ui->stackedWidget->widget(m_gamePageIndex))) {
            gw->updateTable(piles); gw->updateHand(hand);
            gw->setStatusMessage(state.current_player_id() == m_userId ? "Rândul tău!" : "Așteaptă...");
        }
    }
}