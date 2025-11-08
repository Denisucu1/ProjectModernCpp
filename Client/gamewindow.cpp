#include "gamewindow.h"
#include "ui_gamewindow.h"

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QWidget#GameWindow { background-color: #014421; }");
}

GameWindow::~GameWindow()
{
    delete ui;
}
