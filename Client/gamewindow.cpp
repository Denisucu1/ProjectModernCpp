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

    m_ascendingPile1->setValue(1);
    m_ascendingPile2->setValue(1);
    m_descendingPile1->setValue(100);
    m_descendingPile2->setValue(100);

    QHBoxLayout* pileLayout = new QHBoxLayout(ui->pileContainer);

    pileLayout->addWidget(m_descendingPile1);
    pileLayout->addWidget(m_descendingPile2);
    pileLayout->addWidget(m_ascendingPile1);
    pileLayout->addWidget(m_ascendingPile2);
}

GameWindow::~GameWindow()
{
    delete ui;
}