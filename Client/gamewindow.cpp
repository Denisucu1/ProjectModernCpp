#include "gamewindow.h"
#include "ui_gamewindow.h"
#include <QHBoxLayout>

GameWindow::GameWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QWidget#GameWindow { background-color: #014421; }");

    m_ascendingPile1 = new CustomCard(this);
    m_ascendingPile2 = new CustomCard(this);
    m_descendingPile1 = new CustomCard(this);
    m_descendingPile2 = new CustomCard(this);
    m_drawPile = new CustomCard(this);

    m_ascendingPile1->setValue(1);
    m_ascendingPile2->setValue(1);
    m_descendingPile1->setValue(100);
    m_descendingPile2->setValue(100);
    m_drawPile->setFaceDown(true);

    if (ui->pileContainer->layout() == nullptr) {
        QHBoxLayout* pileLayout = new QHBoxLayout(ui->pileContainer);
        pileLayout->addWidget(m_drawPile);
        pileLayout->addWidget(m_descendingPile1);
        pileLayout->addWidget(m_descendingPile2);
        pileLayout->addWidget(m_ascendingPile1);
        pileLayout->addWidget(m_ascendingPile2);
    }
    else {
        ui->pileContainer->layout()->addWidget(m_drawPile);
        ui->pileContainer->layout()->addWidget(m_descendingPile1);
        ui->pileContainer->layout()->addWidget(m_descendingPile2);
        ui->pileContainer->layout()->addWidget(m_ascendingPile1);
        ui->pileContainer->layout()->addWidget(m_ascendingPile2);
    }

    int cardsInHand = 6;
    for (int i = 0; i < cardsInHand; ++i)
    {
        CustomCard* newCard = new CustomCard(this);
        newCard->setValue(0);
        ui->handLayout->addWidget(newCard);
        m_handCards.push_back(newCard);
    }
}

GameWindow::~GameWindow()
{
    delete ui;
}