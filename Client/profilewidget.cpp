#include "profilewidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>

ProfileWidget::ProfileWidget(const QString& username, QWidget* parent) : QWidget(parent) {
    setupLayout(username);
}

void ProfileWidget::setupLayout(const QString& username) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();

    QWidget* profileCard = new QWidget();
    profileCard->setObjectName("profileCard");
    profileCard->setFixedWidth(400);

    QVBoxLayout* cardLayout = new QVBoxLayout(profileCard);
    cardLayout->setContentsMargins(30, 30, 30, 30);

    QLabel* nameHeader = new QLabel(username.toUpper());
    nameHeader->setStyleSheet("font-size: 24px; font-weight: bold; color: #4CAF50;");
    nameHeader->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(nameHeader);

    QGridLayout* statsGrid = new QGridLayout();
    auto addStatRow = [&](const QString& text, QLabel*& label, int row) {
        QLabel* title = new QLabel(text);
        title->setStyleSheet("color: #AAAAAA; font-size: 14px;");
        label = new QLabel("0");
        label->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
        statsGrid->addWidget(title, row, 0, Qt::AlignLeft);
        statsGrid->addWidget(label, row, 1, Qt::AlignRight);
        };

    addStatRow("Games Played", m_gamesPlayedLabel, 0);
    addStatRow("Games Won", m_gamesWonLabel, 1);
    addStatRow("Cards at Loss", m_cardsAtLossLabel, 2);
    addStatRow("Total Time (min)", m_totalTimeLabel, 3);
    addStatRow("Performance Score", m_performanceScoreLabel, 4);

    cardLayout->addLayout(statsGrid);
    mainLayout->addWidget(profileCard, 0, Qt::AlignCenter);
    mainLayout->addStretch();
}

void ProfileWidget::updateStats(const QJsonObject& json) {
    m_gamesPlayedLabel->setText(QString::number(json["gamesPlayed"].toInt()));
    m_gamesWonLabel->setText(QString::number(json["gamesWon"].toInt()));
    m_totalTimeLabel->setText(QString::number(json["hoursPlayed"].toInt()));
    m_cardsAtLossLabel->setText(QString::number(json["cardsLeftOnLosses"].toInt()));
    m_performanceScoreLabel->setText(QString::number(json["performanceScore"].toDouble(), 'f', 2));
}