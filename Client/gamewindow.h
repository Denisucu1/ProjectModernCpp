#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QDateTime>
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

    void appendChatMessage(int senderId, const QString& text);

signals:
    void playerMoved(int cardValue, int stackIndex);
    void chatMessageSent(const QString& text);

private slots:
    void onCardClicked(CustomCard* card);
    void onEndTurnClicked();
    void onSendChatClicked();

private:
    void setupUI();
    void setupTopBar(QVBoxLayout* mainLayout);
    void setupTableArea(QVBoxLayout* mainLayout);
    void setupHandArea(QVBoxLayout* mainLayout);
    void setupChatUI(QHBoxLayout* mainLayout);
    void configurePile(CustomCard*& pile, CustomCard::CardType type, int value, int index);

    QLabel* m_statusLabel;
    QPushButton* m_endTurnButton;
    QWidget* m_handContainer;
    QHBoxLayout* m_handLayout;

    QTextEdit* m_chatDisplay;
    QLineEdit* m_chatInput;

    CustomCard* m_pileDesc1;
    CustomCard* m_pileDesc2;
    CustomCard* m_pileAsc1;
    CustomCard* m_pileAsc2;
    CustomCard* m_drawPile;

    CustomCard* m_selectedCardInHand;

    const int minWindowWidth = 850;
    const int minWindowHeight = 600;
    const int mainLayoutPadding = 20;
    const int tableSpacing = 15;
    const int handAreaHeight = 160;
    const int statusFontSize = 20;
    const int stretchSmall = 1;
    const int stretchLarge = 2;

    const int stackIndexAsc1 = 0;
    const int stackIndexAsc2 = 1;
    const int stackIndexDesc1 = 2;
    const int stackIndexDesc2 = 3;

    const int initialAscValue = 1;
    const int initialDescValue = 100;

    const QString endTurnStyle = "QPushButton { background-color: #d32f2f; color: white; padding: 10px 20px; border-radius: 5px; font-weight: bold; }";
    const QString statusStyle = "color: white; font-size: 20px; font-weight: bold;";
    const QString handContainerStyle = "background-color: rgba(255, 255, 255, 0.1); border-radius: 15px;";
};

#endif