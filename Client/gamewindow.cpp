#include "gamewindow.h"
#include <QMessageBox>

GameWindow::GameWindow(QWidget* parent) : QWidget(parent), m_selectedCardInHand(nullptr) {
    setupUI();
}

GameWindow::~GameWindow() {}

void GameWindow::setupUI() {
    this->setMinimumSize(minWindowWidth, minWindowHeight);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(mainLayoutPadding, mainLayoutPadding, mainLayoutPadding, mainLayoutPadding);

    setupTopBar(mainLayout);
    mainLayout->addStretch(stretchSmall);
    setupTableArea(mainLayout);
    mainLayout->addStretch(stretchSmall);
    setupHandArea(mainLayout);
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
    m_statusLabel->setText(enabled ? "Your turn" : "Opponent's turn");
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