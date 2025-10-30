#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include <QApplication>

MainMenu::MainMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainMenu)
{
    ui->setupUi(this);
}

MainMenu::~MainMenu()
{
    delete ui;
}

void MainMenu::on_playButton_clicked()
{
    GameWindow *game = new GameWindow(this);
    game->show();
    this->close();
}


void MainMenu::on_profileButton_clicked()
{

}


void MainMenu::on_exitButton_clicked()
{
    QApplication::quit();
}

