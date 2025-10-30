#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "gamewindow.h"
#include <QApplication>
#include "profilewindow.h"

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
    ProfileWindow *profile = new ProfileWindow(this);
    profile->show();
}


void MainMenu::on_exitButton_clicked()
{
    QApplication::quit();
}

