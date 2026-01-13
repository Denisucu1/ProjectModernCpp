#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include "gameclient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFrame>
#include "GameProtocol.pb.h"

MainMenu::MainMenu(const QString& username, int userId, QWidget* parent) :
    QWidget(parent), ui(new Ui::MainMenu), m_username(username), m_userId(userId)
{
    ui->setupUi(this);

    ui->roomCodeInput->setFixedWidth(300);
    ui->roomCodeInput->setAlignment(Qt::AlignCenter);

    ui->confirmJoinButton->setFixedWidth(300);
    ui->cancelJoinButton->setFixedWidth(300);

    ui->roomCodeInput->setInputMask(">AAAA;");

    QString styleSheet = R"(
        QWidget#MainMenu { background-color: #1e1e1e; }
        QPushButton { 
            background-color: #4a4a4a; color: white; border-radius: 20px; 
            font-weight: bold; font-size: 14px; min-width: 180px; max-width: 180px; min-height: 45px; border: none;
        }
        QPushButton:hover { background-color: #5a5a5a; }
        QPushButton#playButton, QPushButton#createGameButton, QPushButton#joinGameButton, 
        QPushButton#confirmJoinButton, QPushButton#startGameButton { background-color: #4CAF50; }
        QPushButton#exitButton, QPushButton#backButton, QPushButton#cancelJoinButton, 
        QPushButton#lobbyBackButton, QPushButton#modeBackButton, QPushButton#closeLobbyButton { background-color: #D32F2F; }
        QPushButton#profileButton { background-color: #5C6BC0; }
        QLineEdit { padding: 8px; border-radius: 10px; background-color: #2b2b2b; color: white; border: 1px solid #4CAF50; min-width: 180px; }
        QListWidget { background-color: #2b2b2b; color: white; border-radius: 10px; border: 1px solid #4a4a4a; }
        QLabel { color: white; }
    )";
    this->setStyleSheet(styleSheet);

    ui->stackedWidget->setMaximumWidth(16777215);
    ui->stackedWidget->setMinimumWidth(850);
    ui->stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (this->layout()) delete this->layout();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(ui->stackedWidget);
    this->setLayout(mainLayout);

    auto centerPage = [](QWidget* page, const QList<QWidget*>& widgets) {
        if (page->layout()) delete page->layout();
        QVBoxLayout* layout = new QVBoxLayout(page);
        layout->addStretch();
        for (QWidget* w : widgets) { layout->addWidget(w, 0, Qt::AlignCenter); }
        layout->addStretch();
        layout->setSpacing(20);
        };

    centerPage(ui->menuPage, { ui->playButton, ui->profileButton, ui->exitButton });
    centerPage(ui->page, { ui->createGameButton, ui->joinGameButton, ui->modeBackButton });

    if (ui->profilePage->layout()) delete ui->profilePage->layout();

    for (auto label : ui->profilePage->findChildren<QLabel*>()) { label->hide(); }

    QVBoxLayout* profVLayout = new QVBoxLayout(ui->profilePage);
    profVLayout->addStretch();

    QWidget* profileCard = new QWidget();
    profileCard->setObjectName("profileCard");
    profileCard->setStyleSheet("QWidget#profileCard { background-color: #252525; border: 2px solid #4CAF50; border-radius: 20px; }");
    profileCard->setFixedWidth(450);

    QVBoxLayout* cardLayout = new QVBoxLayout(profileCard);
    cardLayout->setContentsMargins(30, 30, 30, 30);
    cardLayout->setSpacing(20);

    QLabel* nameHeader = new QLabel(m_username.toUpper());
    nameHeader->setStyleSheet("font-size: 24px; font-weight: bold; color: #4CAF50; border: none;");
    nameHeader->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(nameHeader);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #444444; border: none; min-height: 1px;");
    cardLayout->addWidget(line);

    QGridLayout* statsGrid = new QGridLayout();
    statsGrid->setSpacing(15);

    auto addStyledStat = [&](const QString& labelText, QWidget* valueLabel, int row) {
        QLabel* l = new QLabel(labelText);
        l->setStyleSheet("color: #AAAAAA; font-size: 14px; border: none;");
        l->show();
        valueLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold; border: none;");
        valueLabel->show();
        statsGrid->addWidget(l, row, 0, Qt::AlignLeft);
        statsGrid->addWidget(valueLabel, row, 1, Qt::AlignRight);
        };

    addStyledStat("Jocuri Jucate", ui->gamesPlayedLabel, 0);
    addStyledStat("Jocuri Castigate", ui->gamesWonLabel, 1);
    addStyledStat("Carti la infrangeri", ui->cardsAtLossLabel, 2);
    addStyledStat("Timp total (min)", ui->totalTimeLabel, 3);
    addStyledStat("Scor Performanta", ui->performanceScoreLabel, 4);

    cardLayout->addLayout(statsGrid);

    QHBoxLayout* cardHCenter = new QHBoxLayout();
    cardHCenter->addStretch();
    cardHCenter->addWidget(profileCard);
    cardHCenter->addStretch();

    profVLayout->addLayout(cardHCenter);
    profVLayout->addSpacing(30);
    ui->backButton->show();
    profVLayout->addWidget(ui->backButton, 0, Qt::AlignCenter);
    profVLayout->addStretch();

    if (ui->lobbyPage->layout()) delete ui->lobbyPage->layout();
    QVBoxLayout* joinLayout = new QVBoxLayout(ui->lobbyPage);
    joinLayout->addStretch();
    QHBoxLayout* hRow = new QHBoxLayout();
    hRow->setAlignment(Qt::AlignCenter);
    hRow->addWidget(ui->roomCodeInput);
    hRow->addWidget(ui->confirmJoinButton);
    joinLayout->addLayout(hRow);
    joinLayout->addWidget(ui->cancelJoinButton, 0, Qt::AlignCenter);
    joinLayout->addStretch();

    if (ui->page_2->layout()) delete ui->page_2->layout();
    QVBoxLayout* lobbyWaitLayout = new QVBoxLayout(ui->page_2);
    lobbyWaitLayout->addStretch();
    lobbyWaitLayout->addWidget(ui->roomCodeTitleLabel, 0, Qt::AlignCenter);
    lobbyWaitLayout->addWidget(ui->generatedCodeLabel, 0, Qt::AlignCenter);
    lobbyWaitLayout->addWidget(ui->lobbyPlayersList, 0, Qt::AlignCenter);
    lobbyWaitLayout->addWidget(ui->startGameButton, 0, Qt::AlignCenter);
    lobbyWaitLayout->addWidget(ui->closeLobbyButton, 0, Qt::AlignCenter);
    lobbyWaitLayout->addStretch();

    ui->lobbyBackButton->hide();

    m_networkManager = new QNetworkAccessManager(this);
    GameWindow* gamePage = new GameWindow(this);
    m_gamePageIndex = ui->stackedWidget->addWidget(gamePage);

    ui->stackedWidget->setCurrentIndex(0);
    ui->usernameLabel->setText(m_username);

    m_gameClient = new GameClient(QUrl("ws://localhost:18080/ws/game"), m_userId, this);
    connect(m_gameClient, &GameClient::connected, this, [this]() {
        QJsonObject login; login["type"] = "login"; login["userId"] = m_userId;
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
            action->set_card_value(cardValue); action->set_stack_index(stackIndex);
        }
        m_gameClient->sendBinaryMessage(QByteArray::fromStdString(msg.SerializeAsString()));
        });

    m_gameClient->connectToServer();
}

void MainMenu::onSocketMessage(const QString& message) {
    QJsonObject json = QJsonDocument::fromJson(message.toUtf8()).object();

    if (json.contains("players")) {
        ui->lobbyPlayersList->clear();
        QJsonArray players = json["players"].toArray();
        for (const auto& val : players) {
            QJsonObject p = val.toObject();
            QString text = p["username"].toString() + " (ID: " + QString::number(p["userId"].toInt()) + ")";
            ui->lobbyPlayersList->addItem(text);
        }
    }

    if (json.contains("status")) {
        QString status = json["status"].toString();
        if (status == "room_created") {
            ui->generatedCodeLabel->setText(json["roomCode"].toString());
            ui->lobbyPlayersList->clear();
            ui->lobbyPlayersList->addItem(m_username + " (ID: " + QString::number(m_userId) + ")"); // Fix listă Host
            ui->stackedWidget->setCurrentIndex(4);
        }
        else if (status == "joined_room") {
            ui->generatedCodeLabel->setText(json["roomCode"].toString());
            ui->stackedWidget->setCurrentIndex(4);
        }
    }
}

void MainMenu::on_closeLobbyButton_clicked() {
    QJsonObject leaveMsg;
    leaveMsg["type"] = "leave_room";
    leaveMsg["userId"] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(leaveMsg).toJson(QJsonDocument::Compact));
    ui->stackedWidget->setCurrentIndex(2);
    ui->lobbyPlayersList->clear();
}

void MainMenu::on_lobbyBackButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainMenu::onGameBinaryMessage(const QByteArray& data) {
    if (ui->stackedWidget->currentIndex() == 4) ui->stackedWidget->setCurrentIndex(m_gamePageIndex);
    myproject::GameMessage msg;
    if (!msg.ParseFromArray(data.constData(), data.size())) return;

    if (msg.type() == myproject::GameMessage_Type_GAME_OVER) {
        QMessageBox::information(this, "Final", msg.state().game_status() == 1 ? "Victorie!" : "Infrangere!");
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }

    if (msg.type() == myproject::GameMessage_Type_STATE_UPDATE && msg.has_state()) {
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
            gw->setInteractionEnabled(state.current_player_id() == m_userId);
        }
    }
}

void MainMenu::on_playButton_clicked() { ui->stackedWidget->setCurrentIndex(2); }
void MainMenu::on_exitButton_clicked() { QApplication::quit(); }
void MainMenu::on_backButton_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void MainMenu::on_modeBackButton_clicked() { ui->stackedWidget->setCurrentIndex(0); }
void MainMenu::on_cancelJoinButton_clicked() { ui->stackedWidget->setCurrentIndex(2); }

void MainMenu::on_createGameButton_clicked() {
    QJsonObject json; json["type"] = "create_room"; json["userId"] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void MainMenu::on_joinGameButton_clicked() { ui->stackedWidget->setCurrentIndex(3); }

void MainMenu::on_confirmJoinButton_clicked() {
    QString code = ui->roomCodeInput->text().toUpper().trimmed();
    if (code.length() != 4) return;
    QJsonObject json; json["type"] = "join_room"; json["userId"] = m_userId; json["roomCode"] = code;
    m_gameClient->sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void MainMenu::on_startGameButton_clicked() {
    QJsonObject json; json["type"] = "start_game"; json["userId"] = m_userId; json["roomCode"] = ui->generatedCodeLabel->text();
    m_gameClient->sendMessage(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void MainMenu::on_profileButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
    QNetworkRequest req(QUrl(QString("http://localhost:18080/api/profile/%1").arg(m_userId)));
    QNetworkReply* reply = m_networkManager->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onProfileReply(reply); });
}

void MainMenu::onProfileReply(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        ui->gamesPlayedLabel->setText(QString::number(json["gamesPlayed"].toInt()));
        ui->gamesWonLabel->setText(QString::number(json["gamesWon"].toInt()));
        ui->totalTimeLabel->setText(QString::number(json["hoursPlayed"].toInt()));
        ui->cardsAtLossLabel->setText(QString::number(json["cardsLeftOnLosses"].toInt()));
        ui->performanceScoreLabel->setText(QString::number(json["performanceScore"].toDouble(), 'f', 2));
    }
    reply->deleteLater();
}

MainMenu::~MainMenu() { delete ui; }