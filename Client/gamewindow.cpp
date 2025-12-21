#include "gamewindow.h"
#include "ui_gamewindow.h"
#include <QHBoxLayout>

GameWindow::GameWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    this->setAutoFillBackground(true);
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, QColor("#014421"));
    this->setPalette(pal);

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

    QHBoxLayout* pileLayout = qobject_cast<QHBoxLayout*>(ui->pileContainer->layout());
    if (!pileLayout) {
        pileLayout = new QHBoxLayout(ui->pileContainer);
    }

    pileLayout->setSpacing(100);

    pileLayout->setAlignment(Qt::AlignCenter);

    pileLayout->addWidget(m_drawPile);
    pileLayout->addWidget(m_descendingPile1);
    pileLayout->addWidget(m_descendingPile2);
    pileLayout->addWidget(m_ascendingPile1);
    pileLayout->addWidget(m_ascendingPile2);
}

GameWindow::~GameWindow()
{
    delete ui;
}