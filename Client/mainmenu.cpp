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
    QWidget(parent),
    m_ui(new Ui::MainMenu),
    m_username(username),
    m_userId(userId) {
    m_ui->setupUi(this);

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
    m_gamePageIndex = m_ui->stackedWidget->addWidget(gamePage);
    switchToPage(PageIndex::menu);
}

void MainMenu::setupNetwork() {
    m_network = new NetworkManager(m_wsUrl, m_apiUrl, m_userId, this);

    connect(m_network, &NetworkManager::roomCreated, this, &MainMenu::handleRoomCreated);
    connect(m_network, &NetworkManager::roomJoined, this, &MainMenu::handleRoomJoined);
    connect(m_network, &NetworkManager::roomClosed, this, [this](const QString& reason) {
        QMessageBox::warning(this, m_titleSessionClosed, reason);
        m_ui->lobbyPlayersList->clear();
        switchToPage(PageIndex::menu);
        });
    connect(m_network, &NetworkManager::playerListUpdated, this, &MainMenu::handlePlayerListUpdate);
    connect(m_network, &NetworkManager::gameStateUpdated, this, &MainMenu::handleGameStateUpdate);
    connect(m_network, &NetworkManager::gameOver, this, &MainMenu::handleGameOver);
    connect(m_network, &NetworkManager::profileStatsReceived, this, &MainMenu::handleProfileData);

    if (GameWindow* gw = qobject_cast<GameWindow*>(m_ui->stackedWidget->widget(m_gamePageIndex))) {
        connect(gw, &GameWindow::chatMessageSent, m_network, &NetworkManager::sendChatMessage);
        connect(m_network, &NetworkManager::chatMessageReceived, gw, &GameWindow::appendChatMessage);
        connect(gw, &GameWindow::playerMoved, m_network, &NetworkManager::sendGameAction);
    }

    m_network->connectToServer();
}

void MainMenu::handleRoomCreated(const QString& code) {
    m_ui->generatedCodeLabel->setText(code);
    m_ui->lobbyPlayersList->clear();
    m_ui->lobbyPlayersList->addItem(m_playerDisplayFormat.arg(m_username).arg(m_userId));
    switchToPage(PageIndex::lobbyWait);
}

void MainMenu::handleRoomJoined(const QString& code) {
    m_ui->generatedCodeLabel->setText(code);
    switchToPage(PageIndex::lobbyWait);
}

void MainMenu::handlePlayerListUpdate(const QJsonArray& players) {
    m_ui->lobbyPlayersList->clear();
    for (const auto& val : players) {
        QJsonObject p = val.toObject();
        QString text = m_playerDisplayFormat.arg(p[m_keyUsername].toString())
            .arg(p[m_keyUserId].toInt());
        m_ui->lobbyPlayersList->addItem(text);
    }
}

void MainMenu::handleGameStateUpdate(const myproject::GameState& state) {
    if (m_ui->stackedWidget->currentIndex() == static_cast<int>(PageIndex::lobbyWait)) {
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

    if (GameWindow* gw = qobject_cast<GameWindow*>(m_ui->stackedWidget->widget(m_gamePageIndex))) {
        gw->updateTable(piles);
        gw->updateHand(hand);
        gw->setInteractionEnabled(state.current_player_id() == m_userId);
    }
}

void MainMenu::handleGameOver(bool isVictory) {
    QMessageBox::information(this, m_titleGameOver, isVictory ? m_msgVictory : m_msgDefeat);
    switchToPage(PageIndex::menu);
}

void MainMenu::handleProfileData(const QJsonObject& data) {
    m_profileWidget->updateStats(data);
}

void MainMenu::setupNavigationLayout() {
    m_ui->stackedWidget->setMaximumWidth(m_maxWinWidth);
    m_ui->stackedWidget->setMinimumWidth(m_minWinWidth);
    m_ui->stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (this->layout()) delete this->layout();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(m_marginZero, m_marginZero, m_marginZero, m_marginZero);
    mainLayout->addWidget(m_ui->stackedWidget);
}

void MainMenu::setupMenuPages() {
    auto centerPage = [this](QWidget* page, const QList<QWidget*>& widgets) {
        if (page->layout()) delete page->layout();
        QVBoxLayout* layout = new QVBoxLayout(page);
        layout->addStretch();
        for (QWidget* w : widgets) layout->addWidget(w, 0, Qt::AlignCenter);
        layout->addStretch();
        layout->setSpacing(m_spacingStandard);
        };
    centerPage(m_ui->menuPage, { m_ui->playButton, m_ui->profileButton, m_ui->exitButton });
    centerPage(m_ui->modeSelectionPage, { m_ui->createGameButton, m_ui->joinGameButton, m_ui->modeBackButton });
}

void MainMenu::setupProfilePage() {
    if (m_ui->profilePage->layout()) {
        QLayoutItem* item;
        while ((item = m_ui->profilePage->layout()->takeAt(0)) != nullptr) delete item;
    }
    else {
        new QVBoxLayout(m_ui->profilePage);
    }
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(m_ui->profilePage->layout());
    layout->setContentsMargins(m_marginStandard, m_marginStandard, m_marginStandard, m_marginStandard);
    layout->addStretch(m_stretchFactor);
    m_profileWidget = new ProfileWidget(m_username, this);
    layout->addWidget(m_profileWidget, 0, Qt::AlignCenter);
    layout->addSpacing(m_spacingLarge);
    layout->addWidget(m_ui->backButton, 0, Qt::AlignCenter);
    layout->addStretch(m_stretchFactor);
}

void MainMenu::setupLobbyPages() {
    m_ui->roomCodeInput->setMaxLength(m_roomCodeLength);
    m_ui->roomCodeInput->setAlignment(Qt::AlignCenter);
    connect(m_ui->roomCodeInput, &QLineEdit::textEdited, [this](const QString& text) {
        int pos = m_ui->roomCodeInput->cursorPosition();
        m_ui->roomCodeInput->setText(text.toUpper());
        m_ui->roomCodeInput->setCursorPosition(pos);
        });

    if (m_ui->lobbyPage->layout()) delete m_ui->lobbyPage->layout();
    QVBoxLayout* joinLayout = new QVBoxLayout(m_ui->lobbyPage);
    joinLayout->addStretch();
    QHBoxLayout* hRow = new QHBoxLayout();
    hRow->setAlignment(Qt::AlignCenter);
    hRow->addWidget(m_ui->roomCodeInput);
    hRow->addWidget(m_ui->confirmJoinButton);
    joinLayout->addLayout(hRow);
    joinLayout->addWidget(m_ui->cancelJoinButton, 0, Qt::AlignCenter);
    joinLayout->addStretch();

    if (m_ui->lobbyWaitingPage->layout()) delete m_ui->lobbyWaitingPage->layout();
    QVBoxLayout* waitLayout = new QVBoxLayout(m_ui->lobbyWaitingPage);
    waitLayout->addStretch();
    waitLayout->addWidget(m_ui->roomCodeTitleLabel, 0, Qt::AlignCenter);
    waitLayout->addWidget(m_ui->generatedCodeLabel, 0, Qt::AlignCenter);
    waitLayout->addWidget(m_ui->lobbyPlayersList, 0, Qt::AlignCenter);
    waitLayout->addWidget(m_ui->startGameButton, 0, Qt::AlignCenter);
    waitLayout->addWidget(m_ui->closeLobbyButton, 0, Qt::AlignCenter);
    waitLayout->addStretch();
}

void MainMenu::connectUiSignals() {
    connect(m_ui->playButton, &QPushButton::clicked, this, &MainMenu::onPlayButtonClicked);
    connect(m_ui->exitButton, &QPushButton::clicked, this, &MainMenu::onExitButtonClicked);
    connect(m_ui->backButton, &QPushButton::clicked, this, &MainMenu::onBackButtonClicked);
    connect(m_ui->modeBackButton, &QPushButton::clicked, this, &MainMenu::onModeBackButtonClicked);
    connect(m_ui->cancelJoinButton, &QPushButton::clicked, this, &MainMenu::onCancelJoinButtonClicked);
    connect(m_ui->createGameButton, &QPushButton::clicked, this, &MainMenu::onCreateGameButtonClicked);
    connect(m_ui->joinGameButton, &QPushButton::clicked, this, &MainMenu::onJoinGameButtonClicked);
    connect(m_ui->confirmJoinButton, &QPushButton::clicked, this, &MainMenu::onConfirmJoinButtonClicked);
    connect(m_ui->startGameButton, &QPushButton::clicked, this, &MainMenu::onStartGameButtonClicked);
    connect(m_ui->profileButton, &QPushButton::clicked, this, &MainMenu::onProfileButtonClicked);
    connect(m_ui->closeLobbyButton, &QPushButton::clicked, this, &MainMenu::onCloseLobbyButtonClicked);
}

void MainMenu::switchToPage(PageIndex page) {
    m_ui->stackedWidget->setCurrentIndex(static_cast<int>(page));
}

void MainMenu::onCreateGameButtonClicked() { m_network->createRoom(); }
void MainMenu::onConfirmJoinButtonClicked() {
    QString code = m_ui->roomCodeInput->text().toUpper().trimmed();
    if (code.length() == m_roomCodeLength) m_network->joinRoom(code);
}
void MainMenu::onStartGameButtonClicked() { m_network->startGame(m_ui->generatedCodeLabel->text()); }
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

MainMenu::~MainMenu() { delete m_ui; }