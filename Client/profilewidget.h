#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QJsonObject>

class ProfileWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProfileWidget(const QString& username, QWidget* parent = nullptr);
    void updateStats(const QJsonObject& data);

private:
    void setupLayout(const QString& username);

    QLabel* m_gamesPlayedLabel;
    QLabel* m_gamesWonLabel;
    QLabel* m_cardsAtLossLabel;
    QLabel* m_totalTimeLabel;
    QLabel* m_performanceScoreLabel;

    const int m_cardFixedWidth = 400;
    const int m_cardMargins = 30;
    const int m_spacingLarge = 30;
    const int m_precision = 2;

    const QString m_keyGamesPlayed = "gamesPlayed";
    const QString m_keyGamesWon = "gamesWon";
    const QString m_keyHoursPlayed = "hoursPlayed";
    const QString m_keyCardsLeft = "cardsLeftOnLosses";
    const QString m_keyPerfScore = "performanceScore";

    const QString m_labelGamesPlayed = "Games Played";
    const QString m_labelGamesWon = "Games Won";
    const QString m_labelCardsLoss = "Cards at Loss";
    const QString m_labelTotalTime = "Total Time (min)";
    const QString m_labelPerfScore = "Performance Score";
    const QString m_defaultStatValue = "0";
};

#endif