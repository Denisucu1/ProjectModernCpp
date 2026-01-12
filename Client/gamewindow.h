#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <vector>
#include "CustomCard.h"

class GameWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameWindow(QWidget* parent = nullptr);
    ~GameWindow();

    void updateHand(const std::vector<int>& cardValues);
    void updateTable(const std::vector<int>& pilesTopCards);
    void setStatusMessage(const QString& message);
    void setInteractionEnabled(bool enabled);

signals:
    void playerMoved(int cardValue, int stackIndex);

private slots:
    void onCardClicked(CustomCard* card);
    void onEndTurnClicked();

private:
    void setupUI();

    QLabel* m_statusLabel;
    QPushButton* m_endTurnButton;
    QWidget* m_handContainer;
    QHBoxLayout* m_handLayout;

    CustomCard* m_pileDesc1;
    CustomCard* m_pileDesc2;
    CustomCard* m_pileAsc1;
    CustomCard* m_pileAsc2;
    CustomCard* m_drawPile;

    CustomCard* m_selectedCardInHand;
};

#endif