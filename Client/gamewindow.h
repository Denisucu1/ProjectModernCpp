#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include "customcard.h"
#include <vector>

namespace Ui {
    class GameWindow;
}

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget* parent = nullptr);
    ~GameWindow();

private:
    Ui::GameWindow* ui;

    CustomCard* m_ascendingPile1;
    CustomCard* m_ascendingPile2;
    CustomCard* m_descendingPile1;
    CustomCard* m_descendingPile2;
    CustomCard* m_drawPile;

    std::vector<CustomCard*> m_handCards;
};

#endif