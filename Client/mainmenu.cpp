#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QApplication>

MainMenu::MainMenu(const QString& username, int userId, QWidget* parent) :
    QWidget(parent), ui(new Ui::MainMenu), m_username(username), m_userId(userId) {
    ui->setupUi(this);
    initializeUi();
    setupGameLogic();
    m_gameClient->connectToServer();
}

void MainMenu::initializeUi() {
    setupNavigationLayout();
    setupMenuPages();
    setupProfilePage();
    setupLobbyPages();
}

void MainMenu::setupNavigationLayout() {
    ui->stackedWidget->setMaximumWidth(16777215);
    ui->stackedWidget->setMinimumWidth(850);
    ui->stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (this->layout()) delete this->layout();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(ui->stackedWidget);
}

void MainMenu::setupMenuPages() {
    auto centerPage = [this](QWidget* page, const QList<QWidget*>& widgets) {
        if (page->layout()) delete page->layout();
        QVBoxLayout* layout = new QVBoxLayout(page);
        layout->addStretch();
        for (QWidget* w : widgets) layout->addWidget(w, 0, Qt::AlignCenter);
        layout->addStretch();
        layout->setSpacing(20);
        };
    centerPage(ui->menuPage, { ui->playButton, ui->profileButton, ui->exitButton });
    centerPage(ui->modeSelectionPage, { ui->createGameButton, ui->joinGameButton, ui->modeBackButton });
}

void MainMenu::setupProfilePage() {
    if (ui->profilePage->layout()) {
        QLayoutItem* item;
        while ((item = ui->profilePage->layout()->takeAt(0)) != nullptr) delete item;
    }
    else {
        new QVBoxLayout(ui->profilePage);
    }
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->profilePage->layout());
    layout->setContentsMargins(20, 20, 20, 20);
    layout->addStretch(1);
    m_profileWidget = new ProfileWidget(m_username, this);
    layout->addWidget(m_profileWidget, 0, Qt::AlignCenter);
    layout->addSpacing(30);
    layout->addWidget(ui->backButton, 0, Qt::AlignCenter);
    layout->addStretch(1);
}

void MainMenu::setupLobbyPages() {
    ui->roomCodeInput->setMaxLength(m_roomCodeLength);
    ui->roomCodeInput->setAlignment(Qt::AlignCenter);

    ui->confirmJoinButton->setStyleSheet("QPushButton { text-align: center; }");
    connect(ui->roomCodeInput, &QLineEdit::textEdited, [this](const QString& text) {
        int pos = ui->roomCodeInput->cursorPosition();
        ui->roomCodeInput->setText(text.toUpper());
        ui->roomCodeInput->setCursorPosition(pos);
        });

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

    if (ui->lobbyWaitingPage->layout()) delete ui->lobbyWaitingPage->layout();
    QVBoxLayout* waitLayout = new QVBoxLayout(ui->lobbyWaitingPage);
    waitLayout->addStretch();
    waitLayout->addWidget(ui->roomCodeTitleLabel, 0, Qt::AlignCenter);
    waitLayout->addWidget(ui->generatedCodeLabel, 0, Qt::AlignCenter);
    waitLayout->addWidget(ui->lobbyPlayersList, 0, Qt::AlignCenter);
    waitLayout->addWidget(ui->startGameButton, 0, Qt::AlignCenter);
    waitLayout->addWidget(ui->closeLobbyButton, 0, Qt::AlignCenter);
    waitLayout->addStretch();
}

void MainMenu::setupGameLogic() {
    m_networkManager = new QNetworkAccessManager(this);
    GameWindow* gamePage = new GameWindow(this);
    m_gamePageIndex = ui->stackedWidget->addWidget(gamePage);
    switchToPage(PageIndex::menu);

    m_gameClient = new GameClient(QUrl(m_wsUrl), m_userId, this);
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
}

void MainMenu::onSocketMessage(const QString& message) {
    QJsonObject json = QJsonDocument::fromJson(message.toUtf8()).object();
    if (json["type"].toString() == "room_closed") {
        QMessageBox::warning(this, "Room Closed", json["reason"].toString());
        switchToPage(PageIndex::menu);
        ui->lobbyPlayersList->clear();
        return;
    }
    if (json.contains("players")) updatePlayerList(json["players"].toArray());
    if (json.contains("status")) processRoomStatus(json);
}

void MainMenu::updatePlayerList(const QJsonArray& players) {
    ui->lobbyPlayersList->clear();
    for (const auto& val : players) {
        QJsonObject p = val.toObject();
        QString text = p["username"].toString() + " (ID: " + QString::number(p["userId"].toInt()) + ")";
        ui->lobbyPlayersList->addItem(text);
    }
}

void MainMenu::processRoomStatus(const QJsonObject& json) {
    QString status = json["status"].toString();
    if (status == "room_created") {
        ui->generatedCodeLabel->setText(json["roomCode"].toString());
        ui->lobbyPlayersList->clear();
        ui->lobbyPlayersList->addItem(m_username + " (ID: " + QString::number(m_userId) + ")");
        switchToPage(PageIndex::lobbyWait);
    }
    else if (status == "joined_room") {
        ui->generatedCodeLabel->setText(json["roomCode"].toString());
        switchToPage(PageIndex::lobbyWait);
    }
}

void MainMenu::onGameBinaryMessage(const QByteArray& data) {
    if (ui->stackedWidget->currentIndex() == static_cast<int>(PageIndex::lobbyWait)) {
        switchToPage(PageIndex::game);
    }
    myproject::GameMessage msg;
    if (!msg.ParseFromArray(data.constData(), data.size())) return;
    if (msg.type() == myproject::GameMessage_Type_GAME_OVER) {
        QMessageBox::information(this, "Game Over", msg.state().game_status() == 1 ? "Victory!" : "Defeat!");
        switchToPage(PageIndex::menu);
        return;
    }
    if (msg.type() == myproject::GameMessage_Type_STATE_UPDATE && msg.has_state()) {
        updateGameInterface(msg.state());
    }
}

void MainMenu::updateGameInterface(const myproject::GameState& state) {
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
        gw->updateTable(piles);
        gw->updateHand(hand);
        gw->setInteractionEnabled(state.current_player_id() == m_userId);
    }
}

void MainMenu::onProfileReply(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        m_profileWidget->updateStats(json);
    }
    reply->deleteLater();
}

void MainMenu::switchToPage(PageIndex page) {
    ui->stackedWidget->setCurrentIndex(static_cast<int>(page));
}

void MainMenu::on_playButton_clicked() { switchToPage(PageIndex::modeSelection); }
void MainMenu::on_exitButton_clicked() { QApplication::quit(); }
void MainMenu::on_backButton_clicked() { switchToPage(PageIndex::menu); }
void MainMenu::on_modeBackButton_clicked() { switchToPage(PageIndex::menu); }
void MainMenu::on_cancelJoinButton_clicked() { switchToPage(PageIndex::modeSelection); }
void MainMenu::on_joinGameButton_clicked() { switchToPage(PageIndex::joinRoom); }
void MainMenu::on_createGameButton_clicked() {
    QJsonObject j; j["type"] = "create_room"; j["userId"] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}
void MainMenu::on_confirmJoinButton_clicked() {
    QString code = ui->roomCodeInput->text().toUpper().trimmed();
    if (code.length() != m_roomCodeLength) return;
    QJsonObject j; j["type"] = "join_room"; j["userId"] = m_userId; j["roomCode"] = code;
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}
void MainMenu::on_startGameButton_clicked() {
    QJsonObject j; j["type"] = "start_game"; j["userId"] = m_userId; j["roomCode"] = ui->generatedCodeLabel->text();
    m_gameClient->sendMessage(QJsonDocument(j).toJson(QJsonDocument::Compact));
}
void MainMenu::on_profileButton_clicked() {
    switchToPage(PageIndex::profile);
    QNetworkRequest req(QUrl(m_profileUrlTemplate.arg(m_userId)));
    QNetworkReply* reply = m_networkManager->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onProfileReply(reply); });
}
void MainMenu::on_closeLobbyButton_clicked() {
    QJsonObject leaveMsg; leaveMsg["type"] = "leave_room"; leaveMsg["userId"] = m_userId;
    m_gameClient->sendMessage(QJsonDocument(leaveMsg).toJson(QJsonDocument::Compact));
    switchToPage(PageIndex::modeSelection);
}
MainMenu::~MainMenu() { delete ui; }