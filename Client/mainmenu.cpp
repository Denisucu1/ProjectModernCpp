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
    connectUiSignals();

    setupNetwork();
}

void MainMenu::initializeUi() {
    setupNavigationLayout();
    setupMenuPages();
    setupProfilePage();
    setupLobbyPages();

    GameWindow* gamePage = new GameWindow(this);
    m_gamePageIndex = ui->stackedWidget->addWidget(gamePage);
    switchToPage(PageIndex::menu);
}

void MainMenu::setupNetwork() {
    QString wsUrl = "ws://localhost:18080/ws/game";
    QString apiUrl = "http://localhost:18080";

    m_network = new NetworkManager(wsUrl, apiUrl, m_userId, this);

    connect(m_network, &NetworkManager::roomCreated, this, &MainMenu::handleRoomCreated);
    connect(m_network, &NetworkManager::roomJoined, this, &MainMenu::handleRoomJoined);
    connect(m_network, &NetworkManager::roomClosed, this, [this](const QString& reason) {
        QMessageBox::warning(this, "Closed Session", reason);
        ui->lobbyPlayersList->clear();
        switchToPage(PageIndex::menu);
        });
    connect(m_network, &NetworkManager::playerListUpdated, this, &MainMenu::handlePlayerListUpdate);
    connect(m_network, &NetworkManager::gameStateUpdated, this, &MainMenu::handleGameStateUpdate);
    connect(m_network, &NetworkManager::gameOver, this, &MainMenu::handleGameOver);
    connect(m_network, &NetworkManager::profileStatsReceived, this, &MainMenu::handleProfileData);

    if (GameWindow* gw = qobject_cast<GameWindow*>(ui->stackedWidget->widget(m_gamePageIndex))) {
        connect(gw, &GameWindow::chatMessageSent, m_network, &NetworkManager::sendChatMessage);
        connect(m_network, &NetworkManager::chatMessageReceived, gw, &GameWindow::appendChatMessage);
        connect(gw, &GameWindow::playerMoved, m_network, &NetworkManager::sendGameAction);
    }

    m_network->connectToServer();
}

void MainMenu::handleRoomCreated(const QString& code) {
    ui->generatedCodeLabel->setText(code);
    ui->lobbyPlayersList->clear();
    ui->lobbyPlayersList->addItem(m_username + " (ID: " + QString::number(m_userId) + ")");
    switchToPage(PageIndex::lobbyWait);
}

void MainMenu::handleRoomJoined(const QString& code) {
    ui->generatedCodeLabel->setText(code);
    switchToPage(PageIndex::lobbyWait);
}

void MainMenu::handlePlayerListUpdate(const QJsonArray& players) {
    ui->lobbyPlayersList->clear();
    for (const auto& val : players) {
        QJsonObject p = val.toObject();
        QString text = p["username"].toString() + " (ID: " + QString::number(p["userId"].toInt()) + ")";
        ui->lobbyPlayersList->addItem(text);
    }
}

void MainMenu::handleGameStateUpdate(const myproject::GameState& state) {
    if (ui->stackedWidget->currentIndex() == static_cast<int>(PageIndex::lobbyWait)) {
        switchToPage(PageIndex::game);
    }

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

void MainMenu::handleGameOver(bool isVictory) {
    QMessageBox::information(this, "Game Over", isVictory ? "Victory!" : "Defeat!");
    switchToPage(PageIndex::menu);
}

void MainMenu::handleProfileData(const QJsonObject& data) {
    m_profileWidget->updateStats(data);
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

void MainMenu::connectUiSignals() {
    connect(ui->playButton, &QPushButton::clicked, this, &MainMenu::onPlayButtonClicked);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainMenu::onExitButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &MainMenu::onBackButtonClicked);
    connect(ui->modeBackButton, &QPushButton::clicked, this, &MainMenu::onModeBackButtonClicked);
    connect(ui->cancelJoinButton, &QPushButton::clicked, this, &MainMenu::onCancelJoinButtonClicked);
    connect(ui->createGameButton, &QPushButton::clicked, this, &MainMenu::onCreateGameButtonClicked);
    connect(ui->joinGameButton, &QPushButton::clicked, this, &MainMenu::onJoinGameButtonClicked);
    connect(ui->confirmJoinButton, &QPushButton::clicked, this, &MainMenu::onConfirmJoinButtonClicked);
    connect(ui->startGameButton, &QPushButton::clicked, this, &MainMenu::onStartGameButtonClicked);
    connect(ui->profileButton, &QPushButton::clicked, this, &MainMenu::onProfileButtonClicked);
    connect(ui->closeLobbyButton, &QPushButton::clicked, this, &MainMenu::onCloseLobbyButtonClicked);
}

void MainMenu::switchToPage(PageIndex page) {
    ui->stackedWidget->setCurrentIndex(static_cast<int>(page));
}

void MainMenu::onCreateGameButtonClicked() { m_network->createRoom(); }
void MainMenu::onConfirmJoinButtonClicked() {
    QString code = ui->roomCodeInput->text().toUpper().trimmed();
    if (code.length() == m_roomCodeLength) m_network->joinRoom(code);
}
void MainMenu::onStartGameButtonClicked() { m_network->startGame(ui->generatedCodeLabel->text()); }
void MainMenu::onCloseLobbyButtonClicked() {
    m_network->leaveRoom();
    switchToPage(PageIndex::modeSelection);
}
void MainMenu::onProfileButtonClicked() {
    switchToPage(PageIndex::profile);
    m_network->fetchProfileStats();
}

void MainMenu::onPlayButtonClicked() { switchToPage(PageIndex::modeSelection); }
void MainMenu::onExitButtonClicked() { QApplication::quit(); }
void MainMenu::onBackButtonClicked() { switchToPage(PageIndex::menu); }
void MainMenu::onModeBackButtonClicked() { switchToPage(PageIndex::menu); }
void MainMenu::onCancelJoinButtonClicked() { switchToPage(PageIndex::modeSelection); }
void MainMenu::onJoinGameButtonClicked() { switchToPage(PageIndex::joinRoom); }

MainMenu::~MainMenu() { delete ui; }