#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include "gameclient.h"
#include <QApplication>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QJsonArray>
#include <QListWidgetItem>
#include <QVBoxLayout>

// --- INCLUDE CRITIC PENTRU PROTOBUF ---
// Asigură-te că acest fișier există în proiectul tău Client!
// Numele poate varia în funcție de cum a fost generat (ex: game.pb.h sau messages.pb.h)
#include "GameProtocol.pb.h"

// --------------------------------------

MainMenu::MainMenu(const QString& username, int userId, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainMenu),
    m_username(username),
    m_userId(userId)
{
    ui->setupUi(this);

    // FIX LAYOUT: Umplem ecranul
    if (!this->layout()) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(ui->stackedWidget);
        this->setLayout(layout);
    }

    // Stilizare CSS
    QString styleSheet = R"(
        QWidget#MainMenu { background-color: #1e1e1e; }
        QPushButton { background-color: #4a4a4a; color: white; border-radius: 6px; padding: 8px; font-weight: bold; }
        QPushButton:hover { background-color: #616161; }
        QPushButton#playButton, QPushButton#createGameButton, QPushButton#joinGameButton, QPushButton#confirmJoinButton, QPushButton#startGameButton { background-color: #4CAF50; }
        QPushButton#exitButton, QPushButton#backButton { background-color: #F44336; }
        QLineEdit { padding: 5px; border-radius: 4px; }
        QLabel { color: white; }
    )";
    this->setStyleSheet(styleSheet);

    m_networkManager = new QNetworkAccessManager(this);

    // 1. Inițializăm GameWindow
    GameWindow* gamePage = new GameWindow(this);
    m_gamePageIndex = ui->stackedWidget->addWidget(gamePage);

    ui->stackedWidget->setCurrentIndex(0);
    ui->usernameLabel->setText(m_username);

    // 2. WebSocket
    QUrl wsUrl("ws://localhost:18080/ws/game");
    m_gameClient = new GameClient(wsUrl, m_userId, this);

    connect(m_gameClient, &GameClient::messageReceived, this, &MainMenu::onSocketMessage);
    connect(m_gameClient, &GameClient::binaryMessageReceived, this, &MainMenu::onGameBinaryMessage);

    // 3. TRIMITERE MUTARE (Serializare Protobuf)
    connect(gamePage, &GameWindow::playerMoved, this, [this](int cardValue, int stackIndex) {

        // Construim mesajul folosind clasele generate de Protobuf
        myproject::GameMessage msg;
        msg.set_type(myproject::GameMessage::ACTION);

        auto* action = msg.mutable_action();

        if (cardValue == 0) {
            // Cod pentru End Turn (verifică enum-ul din .pb.h tău)
            action->set_action_type(myproject::PlayerAction::END_TURN);
        }
        else {
            // Mutare normală
            action->set_action_type(myproject::PlayerAction::PLAY_CARD);
            action->set_card_id(cardValue);
            action->set_stack_index(stackIndex);
        }

        // Serializăm în string binar
        std::string binaryString = msg.SerializeAsString();

        // Trimitem la server
        m_gameClient->sendBinaryMessage(QByteArray::fromStdString(binaryString));
        });

    m_gameClient->connectToServer();
}

MainMenu::~MainMenu()
{
    delete ui;
}

// --- BUTOANE ---
void MainMenu::on_playButton_clicked() { ui->stackedWidget->setCurrentIndex(2); }
void MainMenu::on_exitButton_clicked() { QApplication::quit(); }
void MainMenu::on_backButton_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void MainMenu::on_lobbyBackButton_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void MainMenu::on_modeBackButton_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void MainMenu::on_cancelJoinButton_clicked() { ui->stackedWidget->setCurrentIndex(2); }

void MainMenu::on_createGameButton_clicked() {
    QJsonObject json; json["type"] = "create_room"; json["userId"] = m_userId;
    QJsonDocument doc(json); m_gameClient->sendMessage(doc.toJson(QJsonDocument::Compact));
    ui->createGameButton->setEnabled(false);
}

void MainMenu::on_joinGameButton_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
    ui->roomCodeInput->clear(); ui->roomCodeInput->setFocus();
}

void MainMenu::on_confirmJoinButton_clicked() {
    QString code = ui->roomCodeInput->text().toUpper().trimmed();
    if (code.length() != 4) return;
    QJsonObject json; json["type"] = "join_room"; json["userId"] = m_userId; json["roomCode"] = code;
    QJsonDocument doc(json); m_gameClient->sendMessage(doc.toJson(QJsonDocument::Compact));
    ui->confirmJoinButton->setEnabled(false);
}

void MainMenu::on_startGameButton_clicked() {
    // Trimitem JSON, dar serverul va răspunde cu BINAR dacă reușește
    QJsonObject json; json["type"] = "start_game"; json["userId"] = m_userId; json["roomCode"] = ui->generatedCodeLabel->text();
    QJsonDocument doc(json); m_gameClient->sendMessage(doc.toJson(QJsonDocument::Compact));
}

void MainMenu::on_closeLobbyButton_clicked() {
    QJsonObject json; json["type"] = "leave_room"; json["userId"] = m_userId;
    QJsonDocument doc(json); m_gameClient->sendMessage(doc.toJson(QJsonDocument::Compact));
    ui->stackedWidget->setCurrentIndex(2);
}

void MainMenu::on_profileButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    QNetworkRequest req(QUrl(QString("http://localhost:18080/api/profile/%1").arg(m_userId)));
    connect(m_networkManager->get(req), &QNetworkReply::finished, this, [this, reply = m_networkManager->get(req)]() { onProfileReply(reply); });
}

void MainMenu::onProfileReply(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        auto json = QJsonDocument::fromJson(reply->readAll()).object();
        ui->gamesPlayedLabel->setText(QString::number(json["gamesPlayed"].toInt()));
        ui->gamesWonLabel->setText(QString::number(json["gamesWon"].toInt()));
    }
    reply->deleteLater();
}

// --- RECEPȚIE MESAJE TEXT (Lobby) ---
void MainMenu::onSocketMessage(const QString& message)
{
    QJsonObject json = QJsonDocument::fromJson(message.toUtf8()).object();
    ui->createGameButton->setEnabled(true);
    ui->confirmJoinButton->setEnabled(true);

    if (json.contains("players")) {
        ui->lobbyPlayersList->clear();
        for (const auto& val : json["players"].toArray()) {
            QJsonObject p = val.toObject();
            QString name = p["username"].toString();
            if (name.isEmpty()) name = "Player " + QString::number(p["userId"].toInt());
            QString text = name + " (ID: " + QString::number(p["userId"].toInt()) + ")";
            if (p["userId"].toInt() == m_userId) text += " (Tu)";
            ui->lobbyPlayersList->addItem(new QListWidgetItem(text));
        }
    }

    if (json.contains("status")) {
        QString status = json["status"].toString();
        if (status == "room_created" || status == "joined_room") {
            ui->generatedCodeLabel->setText(json["roomCode"].toString());
            ui->stackedWidget->setCurrentIndex(4);
            // Dacă e host și tocmai a creat camera, se adaugă pe sine în listă
            if (status == "room_created") {
                ui->lobbyPlayersList->clear();
                ui->lobbyPlayersList->addItem(new QListWidgetItem(QString("%1 (ID: %2) (Tu)").arg(m_username).arg(m_userId)));
            }
        }
    }
}

// --- RECEPȚIE MESAJE BINARE (Gameplay - Protobuf) ---
void MainMenu::onGameBinaryMessage(const QByteArray& data)
{
    if (ui->stackedWidget->currentIndex() == 4) {
        ui->stackedWidget->setCurrentIndex(m_gamePageIndex);
    }

    myproject::GameMessage msg;
    if (!msg.ParseFromArray(data.constData(), data.size())) return;

    if (msg.type() == myproject::GameMessage::STATE_UPDATE && msg.has_state()) {
        const auto& state = msg.state();

        // A. Actualizăm Teancurile
        std::vector<int> piles;
        for (int val : state.stack_tops()) {
            piles.push_back(val);
        }

        // B. Găsim mâna jucătorului curent (Noi)
        std::vector<int> hand;
        int currentTurnUserId = state.user_id(); // ID-ul jucătorului care trebuie să mute

        for (const auto& playerInfo : state.players()) {
            if (playerInfo.user_id() == m_userId) {
                // Acesta sunt eu, îmi iau cărțile
                for (int cardVal : playerInfo.your_cards()) {
                    hand.push_back(cardVal);
                }
                break;
            }
        }

        // C. Actualizăm UI-ul GameWindow
        if (QWidget* w = ui->stackedWidget->widget(m_gamePageIndex)) {
            if (GameWindow* gw = qobject_cast<GameWindow*>(w)) {
                gw->updateTable(piles);
                gw->updateHand(hand);

                // Mesaj de stare
                if (currentTurnUserId == m_userId) {
                    gw->setStatusMessage("Este rândul tău!");
                }
                else {
                    gw->setStatusMessage(QString("Așteaptă rândul jucătorului %1").arg(currentTurnUserId));
                }
            }
        }
    }
}