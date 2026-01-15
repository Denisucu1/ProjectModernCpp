#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QJsonObject>

class ProfileWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProfileWidget(const QString& username, QWidget* parent = nullptr);
    void UpdateStats(const QJsonObject& data); 

private:
    void SetupLayout(const QString& username);

    QLabel* m_gamesPlayedLabel;
    QLabel* m_gamesWonLabel;
    QLabel* m_cardsAtLossLabel;
    QLabel* m_totalTimeLabel;
    QLabel* m_performanceScoreLabel;
};

#endif