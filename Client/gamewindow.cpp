#include "gamewindow.h"
#include <QMessageBox>
#include <QScrollBar>

GameWindow::GameWindow(QWidget* parent) : QWidget(parent), m_selectedCardInHand(nullptr) {
    setupUI();
}

GameWindow::~GameWindow() {}

void GameWindow::setupUI() {
    this->setMinimumSize(minWindowWidth, minWindowHeight);

    QHBoxLayout* layoutGlobal = new QHBoxLayout(this);
    layoutGlobal->setContentsMargins(10, 10, 10, 10);
    layoutGlobal->setSpacing(15);

    QWidget* gameAreaContainer = new QWidget(this);
    QVBoxLayout* gameLayout = new QVBoxLayout(gameAreaContainer);
    gameLayout->setContentsMargins(0, 0, 0, 0);

    setupTopBar(gameLayout);
    gameLayout->addStretch(stretchSmall);
    setupTableArea(gameLayout);
    gameLayout->addStretch(stretchSmall);
    setupHandArea(gameLayout);

    layoutGlobal->addWidget(gameAreaContainer, 3); 

    setupChatUI(layoutGlobal);
}

void GameWindow::setupChatUI(QHBoxLayout* mainLayout) {
    QWidget* chatContainer = new QWidget(this);
    chatContainer->setFixedWidth(280); 
    chatContainer->setStyleSheet(
        "QWidget { "
        "  background-color: rgba(30, 30, 30, 230); "
        "  border-radius: 12px; "
        "  border: 1px solid #444; "
        "}"
    );

    QVBoxLayout* chatLayout = new QVBoxLayout(chatContainer);
    chatLayout->setContentsMargins(10, 10, 10, 10);

    QLabel* chatTitle = new QLabel("GAME CHAT", this);
    chatTitle->setStyleSheet(
        "color: #00bfff; "
        "font-weight: bold; "
        "font-size: 14px; "
        "border: none; "
        "letter-spacing: 1px; "
        "padding-bottom: 5px;"
    );
    chatLayout->addWidget(chatTitle);

    m_chatDisplay = new QTextEdit(this);
    m_chatDisplay->setReadOnly(true);
    m_chatDisplay->setStyleSheet(
        "QTextEdit { "
        "  background-color: #121212; "
        "  color: #e0e0e0; "
        "  border: 1px solid #333; "
        "  border-radius: 6px; "
        "  font-family: 'Segoe UI', Arial; "
        "  font-size: 12px; "
        "  padding: 5px;"
        "}"
        "QScrollBar:vertical { border: none; background: #222; width: 8px; }"
        "QScrollBar::handle:vertical { background: #555; border-radius: 4px; }"
    );
    chatLayout->addWidget(m_chatDisplay);

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(8);

    m_chatInput = new QLineEdit(this);
    m_chatInput->setPlaceholderText("Type a message...");
    m_chatInput->setStyleSheet(
        "QLineEdit { "
        "  padding: 8px; "
        "  background: #2a2a2a; "
        "  color: white; "
        "  border: 1px solid #444; "
        "  border-radius: 6px; "
        "}"
        "QLineEdit:focus { border: 1px solid #00bfff; }"
    );

    QPushButton* sendBtn = new QPushButton("Send", this);
    sendBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: #0078d4; "
        "  color: white; "
        "  border-radius: 6px; "
        "  padding: 8px 15px; "
        "  font-weight: bold; "
        "  border: none; "
        "}"
        "QPushButton:hover { "
        "  background-color: #0086f1; "
        "}"
        "QPushButton:pressed { "
        "  background-color: #005a9e; "
        "  padding-top: 9px; padding-left: 16px; " 
        "}"
    );

    inputLayout->addWidget(m_chatInput);
    inputLayout->addWidget(sendBtn);
    chatLayout->addLayout(inputLayout);

    connect(sendBtn, &QPushButton::clicked, this, &GameWindow::onSendChatClicked);
    connect(m_chatInput, &QLineEdit::returnPressed, this, &GameWindow::onSendChatClicked);

    mainLayout->addWidget(chatContainer, 1);
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
    QString color = (senderId == -1) ? "#888" : "#4dabf7";

    m_chatDisplay->append(QString("<span style='color: #555; font-size: 10px;'>[%1]</span> "
        "<b style='color: %2;'>User %3:</b> "
        "<span style='color: #efefef;'>%4</span>")
        .arg(timeStr, color, QString::number(senderId), text));

    m_chatDisplay->verticalScrollBar()->setValue(m_chatDisplay->verticalScrollBar()->maximum());
}

void GameWindow::setupTopBar(QVBoxLayout* mainLayout) {
    QHBoxLayout* topBar = new QHBoxLayout();
    m_statusLabel = new QLabel("Waiting for turn...", this);
    m_statusLabel->setStyleSheet(statusStyle);

    m_endTurnButton = new QPushButton("End Turn", this);
    m_endTurnButton->setStyleSheet(endTurnStyle);
    connect(m_endTurnButton, &QPushButton::clicked, this, &GameWindow::onEndTurnClicked);

    topBar->addWidget(m_statusLabel);
    topBar->addStretch();
    topBar->addWidget(m_endTurnButton);
    mainLayout->addLayout(topBar);
}

void GameWindow::setupTableArea(QVBoxLayout* mainLayout) {
    QWidget* tableArea = new QWidget(this);
    QHBoxLayout* tableLayout = new QHBoxLayout(tableArea);

    configurePile(m_pileAsc1, CustomCard::ASCENDING, initialAscValue, stackIndexAsc1);
    configurePile(m_pileAsc2, CustomCard::ASCENDING, initialAscValue, stackIndexAsc2);

    m_drawPile = new CustomCard(this);
    m_drawPile->setFaceDown(true);
    m_drawPile->setType(CustomCard::DRAW_PILE);

    configurePile(m_pileDesc1, CustomCard::DESCENDING, initialDescValue, stackIndexDesc1);
    configurePile(m_pileDesc2, CustomCard::DESCENDING, initialDescValue, stackIndexDesc2);

    tableLayout->addStretch(stretchLarge);
    tableLayout->addWidget(m_pileAsc1);
    tableLayout->addSpacing(tableSpacing);
    tableLayout->addWidget(m_pileAsc2);
    tableLayout->addStretch(stretchSmall);
    tableLayout->addWidget(m_drawPile);
    tableLayout->addStretch(stretchSmall);
    tableLayout->addWidget(m_pileDesc1);
    tableLayout->addSpacing(tableSpacing);
    tableLayout->addWidget(m_pileDesc2);
    tableLayout->addStretch(stretchLarge);

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
    m_handContainer->setFixedHeight(handAreaHeight);
    m_handContainer->setStyleSheet(handContainerStyle);
    m_handLayout = new QHBoxLayout(m_handContainer);
    m_handLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_handContainer);
}

void GameWindow::updateTable(const std::vector<int>& pilesTopCards) {
    if (pilesTopCards.size() >= 4) {
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
    emit playerMoved(0, 0);
}

void GameWindow::setStatusMessage(const QString& message) {
    m_statusLabel->setText(message);
}