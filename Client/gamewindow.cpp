#include "gamewindow.h"
#include <QMessageBox>

GameWindow::GameWindow(QWidget* parent) :
    QWidget(parent),
    m_selectedCardInHand(nullptr)
{

    setupUI();
}

GameWindow::~GameWindow()
{
}

void GameWindow::setupUI()
{
    this->setStyleSheet("background-color: #2E7D32;");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* topBar = new QHBoxLayout();

    m_statusLabel = new QLabel("Așteptare joc...", this);
    m_statusLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold; background: none; border: none;");

    m_endTurnButton = new QPushButton("End Turn", this);
    m_endTurnButton->setCursor(Qt::PointingHandCursor);
    m_endTurnButton->setStyleSheet(
        "QPushButton { background-color: #d32f2f; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #b71c1c; }"
        "QPushButton:disabled { background-color: #9e9e9e; }"
    );
    m_endTurnButton->setFixedWidth(120);
    connect(m_endTurnButton, &QPushButton::clicked, this, &GameWindow::onEndTurnClicked);

    topBar->addWidget(m_statusLabel);
    topBar->addStretch();
    topBar->addWidget(m_endTurnButton);

    mainLayout->addLayout(topBar);

    mainLayout->addStretch(1);

    QWidget* tableArea = new QWidget(this);
    tableArea->setStyleSheet("background: none;"); 
    QHBoxLayout* tableLayout = new QHBoxLayout(tableArea);
    tableLayout->setSpacing(20);
    tableLayout->setAlignment(Qt::AlignCenter);

    m_pileDesc1 = new CustomCard(this); m_pileDesc1->setValue(100); m_pileDesc1->setType(CustomCard::DESCENDING);
    m_pileDesc2 = new CustomCard(this); m_pileDesc2->setValue(100); m_pileDesc2->setType(CustomCard::DESCENDING);

    m_drawPile = new CustomCard(this);
    m_drawPile->setFaceDown(true);
    m_drawPile->setType(CustomCard::DRAW_PILE);

    m_pileAsc1 = new CustomCard(this); m_pileAsc1->setValue(1); m_pileAsc1->setType(CustomCard::ASCENDING);
    m_pileAsc2 = new CustomCard(this); m_pileAsc2->setValue(1); m_pileAsc2->setType(CustomCard::ASCENDING);

    connect(m_pileDesc1, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);
    connect(m_pileDesc2, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);
    connect(m_pileAsc1, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);
    connect(m_pileAsc2, &CustomCard::cardClicked, this, &GameWindow::onCardClicked);

    tableLayout->addWidget(m_pileDesc1);
    tableLayout->addWidget(m_pileDesc2);
    tableLayout->addSpacing(40);
    tableLayout->addWidget(m_drawPile);
    tableLayout->addSpacing(40);
    tableLayout->addWidget(m_pileAsc1);
    tableLayout->addWidget(m_pileAsc2);

    mainLayout->addWidget(tableArea);
    mainLayout->addStretch(1);

    m_handContainer = new QWidget(this);
    m_handContainer->setFixedHeight(150);
    m_handContainer->setStyleSheet("background-color: rgba(0, 0, 0, 0.2); border-radius: 15px;");

    m_handLayout = new QHBoxLayout(m_handContainer);
    m_handLayout->setAlignment(Qt::AlignCenter);
    m_handLayout->setSpacing(10);

    mainLayout->addWidget(m_handContainer);

    std::vector<int> demoHand = { 10, 25, 98, 4, 12, 66 };
    updateHand(demoHand);
}

void GameWindow::updateHand(const std::vector<int>& cardValues)
{
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

void GameWindow::onCardClicked(CustomCard* card)
{

    if (card->getType() == CustomCard::HAND_CARD) {
        if (m_selectedCardInHand) {
            m_selectedCardInHand->setSelected(false); 
        }
        m_selectedCardInHand = card;
        m_selectedCardInHand->setSelected(true); 
        return;
    }

    if (m_selectedCardInHand && card->getType() != CustomCard::DRAW_PILE) {

        int cardVal = m_selectedCardInHand->getValue();
        int pileVal = card->getValue();
        bool isValid = false;

        if (card->getType() == CustomCard::ASCENDING) {
            if (cardVal > pileVal || cardVal == pileVal - 10) isValid = true;
        }
        else if (card->getType() == CustomCard::DESCENDING) {
            if (cardVal < pileVal || cardVal == pileVal + 10) isValid = true;
        }

        if (isValid) {
            card->setValue(cardVal);

            m_handLayout->removeWidget(m_selectedCardInHand);
            m_selectedCardInHand->deleteLater();
            m_selectedCardInHand = nullptr;

        }
        else {
            QMessageBox::warning(this, "Mutare Invalidă", "Nu poți pune această carte aici!");
        }
    }
}

void GameWindow::updateTable(const std::vector<int>& pilesTopCards) {
    if (pilesTopCards.size() >= 4) {
        m_pileDesc1->setValue(pilesTopCards[0]);
        m_pileDesc2->setValue(pilesTopCards[1]);
        m_pileAsc1->setValue(pilesTopCards[2]);
        m_pileAsc2->setValue(pilesTopCards[3]);
    }
}

void GameWindow::setStatusMessage(const QString& message) {
    m_statusLabel->setText(message);
}

void GameWindow::onEndTurnClicked()
{
    QMessageBox::information(this, "Info", "Ai apăsat End Turn (urmează implementarea rețelei)");
}