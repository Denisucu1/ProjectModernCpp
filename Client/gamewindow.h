#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include "customcard.h"

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget* parent = nullptr);
    ~GameWindow();

    void updateHand(const std::vector<int>& cardValues);
    void updateTable(const std::vector<int>& pilesTopCards);
    void setStatusMessage(const QString& message);

signals:
    void playerMoved(int cardValue, int stackIndex);

private slots:
    void onCardClicked(CustomCard* card);
    void onEndTurnClicked();

private:
    CustomCard* m_pileDesc1;
    CustomCard* m_pileDesc2;
    CustomCard* m_pileAsc1;
    CustomCard* m_pileAsc2;
    CustomCard* m_drawPile;

    QWidget* m_handContainer;
    QHBoxLayout* m_handLayout;
    QLabel* m_statusLabel;
    QPushButton* m_endTurnButton;
    CustomCard* m_selectedCardInHand;

    void setupUI();
};

#endif