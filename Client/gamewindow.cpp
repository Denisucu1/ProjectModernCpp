#include "gamewindow.h"
#include <QMessageBox>
#include <QScrollBar>

GameWindow::GameWindow(QWidget* parent) : QWidget(parent), m_selectedCardInHand(nullptr) {
    setupUI();
}

GameWindow::~GameWindow() {}

void GameWindow::setupUI() {
    this->setMinimumSize(m_minWinWidth, m_minWinHeight);

    QHBoxLayout* layoutGlobal = new QHBoxLayout(this);
    layoutGlobal->setContentsMargins(m_layoutPadding, m_layoutPadding, m_layoutPadding, m_layoutPadding);
    layoutGlobal->setSpacing(m_tableSpacing);

    QWidget* gameAreaContainer = new QWidget(this);
    QVBoxLayout* gameLayout = new QVBoxLayout(gameAreaContainer);
    gameLayout->setContentsMargins(0, 0, 0, 0);

    setupTopBar(gameLayout);
    gameLayout->addStretch(m_stretchSmall);
    setupTableArea(gameLayout);
    gameLayout->addStretch(m_stretchSmall);
    setupHandArea(gameLayout);

    layoutGlobal->addWidget(gameAreaContainer, m_gameAreaStretch);
    setupChatUI(layoutGlobal);
}

void GameWindow::setupChatUI(QHBoxLayout* mainLayout) {
    QWidget* chatContainer = new QWidget(this);
    chatContainer->setObjectName("chatContainer");
    chatContainer->setFixedWidth(m_chatWidth);

    QVBoxLayout* chatLayout = new QVBoxLayout(chatContainer);
    chatLayout->setContentsMargins(m_layoutPadding / 2, m_layoutPadding / 2, m_layoutPadding / 2, m_layoutPadding / 2);

    QLabel* chatTitle = new QLabel("GAME CHAT", this);
    chatTitle->setObjectName("chatTitle");
    chatLayout->addWidget(chatTitle);

    m_chatDisplay = new QTextEdit(this);
    m_chatDisplay->setObjectName("chatDisplay");
    m_chatDisplay->setReadOnly(true);
    chatLayout->addWidget(m_chatDisplay);

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(m_inputSpacing);

    m_chatInput = new QLineEdit(this);
    m_chatInput->setObjectName("chatInput");
    m_chatInput->setPlaceholderText("Type a message...");

    QPushButton* sendBtn = new QPushButton("Send", this);
    sendBtn->setObjectName("sendBtn");

    inputLayout->addWidget(m_chatInput);
    inputLayout->addWidget(sendBtn);
    chatLayout->addLayout(inputLayout);

    connect(sendBtn, &QPushButton::clicked, this, &GameWindow::onSendChatClicked);
    connect(m_chatInput, &QLineEdit::returnPressed, this, &GameWindow::onSendChatClicked);

    mainLayout->addWidget(chatContainer, m_stretchSmall);
}

void GameWindow::onSendChatClicked() {
    QString text = m_chatInput->text().trimmed();
    if (!text.isEmpty()) {
        emit chatMessageSent(text);
        m_chatInput->clear();
    }
}

void GameWindow::appendChatMessage(int senderId, const QString& text) {
    QString timeStr = QDateTime::currentDateTime().toString("HH:mm");
    QString color = m_colorUser;
    QString displayName = m_prefixUser + QString::number(senderId);

    if (senderId == m_systemId) {
        color = m_colorSystem;
        displayName = m_nameSystem;
    }

    m_chatDisplay->append(CHAT_FORMAT.arg(timeStr, color, displayName, text));
    m_chatDisplay->verticalScrollBar()->setValue(m_chatDisplay->verticalScrollBar()->maximum());
}

void GameWindow::setupTopBar(QVBoxLayout* mainLayout) {
    QHBoxLayout* topBar = new QHBoxLayout();
    m_statusLabel = new QLabel("Waiting for turn...", this);
    m_statusLabel->setObjectName("statusLabel");

    m_endTurnButton = new QPushButton("End Turn", this);
    m_endTurnButton->setObjectName("endTurnButton");
    connect(m_endTurnButton, &QPushButton::clicked, this, &GameWindow::onEndTurnClicked);

    topBar->addWidget(m_statusLabel);
    topBar->addStretch();
    topBar->addWidget(m_endTurnButton);
    mainLayout->addLayout(topBar);
}

void GameWindow::setupTableArea(QVBoxLayout* mainLayout) {
    QWidget* tableArea = new QWidget(this);
    QHBoxLayout* tableLayout = new QHBoxLayout(tableArea);

    configurePile(m_pileAsc1, CustomCard::ASCENDING, m_initAscVal, m_stackIdxAsc1);
    configurePile(m_pileAsc2, CustomCard::ASCENDING, m_initAscVal, m_stackIdxAsc2);

    m_drawPile = new CustomCard(this);
    m_drawPile->setFaceDown(true);
    m_drawPile->setType(CustomCard::DRAW_PILE);

    configurePile(m_pileDesc1, CustomCard::DESCENDING, m_initDescVal, m_stackIdxDesc1);
    configurePile(m_pileDesc2, CustomCard::DESCENDING, m_initDescVal, m_stackIdxDesc2);

    tableLayout->addStretch(m_stretchLarge);
    tableLayout->addWidget(m_pileAsc1);
    tableLayout->addSpacing(m_tableSpacing);
    tableLayout->addWidget(m_pileAsc2);
    tableLayout->addStretch(m_stretchSmall);
    tableLayout->addWidget(m_drawPile);
    tableLayout->addStretch(m_stretchSmall);
    tableLayout->addWidget(m_pileDesc1);
    tableLayout->addSpacing(m_tableSpacing);
    tableLayout->addWidget(m_pileDesc2);
    tableLayout->addStretch(m_stretchLarge);

    mainLayout->addWidget(tableArea);
}

void GameWindow::configurePile(CustomCard*& pile, CustomCard::CardType type, int value, int index) {
    pile = new CustomCard(this);
    pile->setType(type);
    pile->setValue(value);
    pile->setProperty("stackIndex", index);
    connect(pile, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);
}

void GameWindow::setupHandArea(QVBoxLayout* mainLayout) {
    m_handContainer = new QWidget(this);
    m_handContainer->setObjectName("handContainer");
    m_handContainer->setFixedHeight(m_handHeight);

    m_handLayout = new QHBoxLayout(m_handContainer);
    m_handLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_handContainer);
}

void GameWindow::updateTable(const std::vector<int>& pilesTopCards) {
    if (pilesTopCards.size() >= static_cast<size_t>(m_pilesCount)) {
        m_pileAsc1->setValue(pilesTopCards[0]);
        m_pileAsc2->setValue(pilesTopCards[1]);
        m_pileDesc1->setValue(pilesTopCards[2]);
        m_pileDesc2->setValue(pilesTopCards[3]);
    }
}

void GameWindow::setInteractionEnabled(bool enabled) {
    m_endTurnButton->setEnabled(enabled);
    m_pileDesc1->setEnabled(enabled);
    m_pileDesc2->setEnabled(enabled);
    m_pileAsc1->setEnabled(enabled);
    m_pileAsc2->setEnabled(enabled);
    m_handContainer->setEnabled(enabled);

    m_statusLabel->setText(enabled ? "Your turn" : "Wait for your turn");
}

void GameWindow::onCardClicked(CustomCard* card) {
    if (card->getType() == CustomCard::HAND_CARD) {
        if (m_selectedCardInHand) {
            m_selectedCardInHand->setSelected(false);
        }
        m_selectedCardInHand = card;
        m_selectedCardInHand->setSelected(true);
    }
    else if (m_selectedCardInHand && card->getType() != CustomCard::DRAW_PILE) {
        emit playerMoved(m_selectedCardInHand->getValue(), card->property("stackIndex").toInt());
    }
}

void GameWindow::updateHand(const std::vector<int>& cardValues) {
    QLayoutItem* item;
    while ((item = m_handLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    m_selectedCardInHand = nullptr;
    for (int val : cardValues) {
        CustomCard* c = new CustomCard(this);
        c->setValue(val);
        c->setType(CustomCard::HAND_CARD);
        connect(c, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);
        m_handLayout->addWidget(c);
    }
}

void GameWindow::onEndTurnClicked() {
    emit playerMoved(m_endTurnVal, m_endTurnVal);
}

void GameWindow::setStatusMessage(const QString& message) {
    m_statusLabel->setText(message);
}