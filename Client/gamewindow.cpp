#include "gamewindow.h"
#include <QMessageBox>

GameWindow::GameWindow(QWidget* parent) : QWidget(parent), m_selectedCardInHand(nullptr) {
    setupUI();
}

GameWindow::~GameWindow() {}

void GameWindow::setupUI() {
    this->setMinimumSize(850, 600);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QHBoxLayout* topBar = new QHBoxLayout();
    m_statusLabel = new QLabel("Așteptare rând...", this);
    m_statusLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");

    m_endTurnButton = new QPushButton("End Turn", this);
    m_endTurnButton->setStyleSheet("QPushButton { background-color: #d32f2f; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }");
    connect(m_endTurnButton, &QPushButton::clicked, this, &GameWindow::onEndTurnClicked);

    topBar->addWidget(m_statusLabel);
    topBar->addStretch();
    topBar->addWidget(m_endTurnButton);
    mainLayout->addLayout(topBar);
    mainLayout->addStretch(1);

    QWidget* tableArea = new QWidget(this);
    QHBoxLayout* tableLayout = new QHBoxLayout(tableArea);

    m_pileAsc1 = new CustomCard(this); m_pileAsc1->setType(CustomCard::ASCENDING); m_pileAsc1->setValue(1); m_pileAsc1->setProperty("stackIndex", 2);
    m_pileAsc2 = new CustomCard(this); m_pileAsc2->setType(CustomCard::ASCENDING); m_pileAsc2->setValue(1); m_pileAsc2->setProperty("stackIndex", 3);

    m_drawPile = new CustomCard(this); m_drawPile->setFaceDown(true); m_drawPile->setType(CustomCard::DRAW_PILE);

    m_pileDesc1 = new CustomCard(this); m_pileDesc1->setType(CustomCard::DESCENDING); m_pileDesc1->setValue(100); m_pileDesc1->setProperty("stackIndex", 0);
    m_pileDesc2 = new CustomCard(this); m_pileDesc2->setType(CustomCard::DESCENDING); m_pileDesc2->setValue(100); m_pileDesc2->setProperty("stackIndex", 1);

    connect(m_pileAsc1, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);
    connect(m_pileAsc2, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);
    connect(m_pileDesc1, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);
    connect(m_pileDesc2, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);

    tableLayout->addStretch(2);
    tableLayout->addWidget(m_pileAsc1); tableLayout->addSpacing(15); tableLayout->addWidget(m_pileAsc2);
    tableLayout->addStretch(1);
    tableLayout->addWidget(m_drawPile);
    tableLayout->addStretch(1);
    tableLayout->addWidget(m_pileDesc1); tableLayout->addSpacing(15); tableLayout->addWidget(m_pileDesc2);
    tableLayout->addStretch(2);

    mainLayout->addWidget(tableArea);
    mainLayout->addStretch(1);

    m_handContainer = new QWidget(this);
    m_handContainer->setFixedHeight(160);
    m_handContainer->setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border-radius: 15px;");
    m_handLayout = new QHBoxLayout(m_handContainer);
    m_handLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_handContainer);
}

void GameWindow::setInteractionEnabled(bool enabled) {
    m_endTurnButton->setEnabled(enabled);
    m_pileDesc1->setEnabled(enabled); m_pileDesc2->setEnabled(enabled);
    m_pileAsc1->setEnabled(enabled); m_pileAsc2->setEnabled(enabled);
    m_handContainer->setEnabled(enabled);
    m_statusLabel->setText(enabled ? "Rândul tău!" : "Așteaptă rândul adversarului...");
}

void GameWindow::onCardClicked(CustomCard* card) {
    if (card->getType() == CustomCard::HAND_CARD) {
        if (m_selectedCardInHand) m_selectedCardInHand->setSelected(false);
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
        if (item->widget()) delete item->widget();
        delete item;
    }
    m_selectedCardInHand = nullptr;
    for (int val : cardValues) {
        CustomCard* c = new CustomCard(this);
        c->setValue(val); c->setType(CustomCard::HAND_CARD);
        connect(c, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);
        m_handLayout->addWidget(c);
    }
}

void GameWindow::updateTable(const std::vector<int>& pilesTopCards) {
    if (pilesTopCards.size() >= 4) {
        m_pileDesc1->setValue(pilesTopCards[0]); m_pileDesc2->setValue(pilesTopCards[1]);
        m_pileAsc1->setValue(pilesTopCards[2]); m_pileAsc2->setValue(pilesTopCards[3]);
    }
}

void GameWindow::onEndTurnClicked() { emit playerMoved(0, 0); }
void GameWindow::setStatusMessage(const QString& message) { m_statusLabel->setText(message); }