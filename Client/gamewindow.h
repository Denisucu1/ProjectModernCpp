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

    const int m_minWinWidth = 850;
    const int m_minWinHeight = 600;
    const int m_layoutPadding = 20;
    const int m_tableSpacing = 15;
    const int m_handHeight = 160;
    const int m_chatWidth = 280;
    const int m_inputSpacing = 8;

    const int m_stretchSmall = 1;
    const int m_stretchLarge = 2;
    const int m_gameAreaStretch = 3;
    const int m_pilesCount = 4;

    const int m_stackIdxAsc1 = 0;
    const int m_stackIdxAsc2 = 1;
    const int m_stackIdxDesc1 = 2;
    const int m_stackIdxDesc2 = 3;

    const int m_initAscVal = 1;
    const int m_initDescVal = 100;
    const int m_systemId = -1;
    const int m_endTurnVal = 0;

    const QString m_colorUser = "#4dabf7";
    const QString m_colorSystem = "#888888";
    const QString m_nameSystem = "System";
    const QString m_prefixUser = "User ";

    const QString m_txtChatTitle = "GAME CHAT";
    const QString m_txtChatPlaceholder = "Type a message...";
    const QString m_txtSend = "Send";
    const QString m_txtEndTurn = "End Turn";
    const QString m_txtWaiting = "Waiting for turn...";
    const QString m_txtYourTurn = "Your turn";
    const QString m_txtWaitTurn = "Wait for your turn";
    const QString m_dateTimeFormat = "HH:mm";

    static inline const QString CHAT_FORMAT =
        "<span style='color: #555; font-size: 10px;'>[%1]</span> "
        "<b style='color: %2;'>%3:</b> "
        "<span style='color: #efefef;'>%4</span>";
};

#endif