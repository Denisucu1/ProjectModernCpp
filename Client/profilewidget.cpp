#include "profilewidget.h"
#include <QVBoxLayout>
#include <QGridLayout>

ProfileWidget::ProfileWidget(const QString& username, QWidget* parent) : QWidget(parent) {
    setupLayout(username);
}

void ProfileWidget::setupLayout(const QString& username) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();

    QWidget* profileCard = new QWidget();
    profileCard->setObjectName("profileCard");
    profileCard->setFixedWidth(m_cardFixedWidth);

    QVBoxLayout* cardLayout = new QVBoxLayout(profileCard);
    cardLayout->setContentsMargins(m_cardMargins, m_cardMargins, m_cardMargins, m_cardMargins);

    QLabel* nameHeader = new QLabel(username.toUpper());
    nameHeader->setObjectName("profileNameHeader");
    nameHeader->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(nameHeader);

    QGridLayout* statsGrid = new QGridLayout();
    auto addStatRow = [&](const QString& text, QLabel*& label, int row) {
        QLabel* title = new QLabel(text);
        title->setProperty("class", "statTitle");

        label = new QLabel(m_defaultStatValue);
        label->setProperty("class", "statValue");

        statsGrid->addWidget(title, row, 0, Qt::AlignLeft);
        statsGrid->addWidget(label, row, 1, Qt::AlignRight);
        };

    addStatRow(m_labelGamesPlayed, m_gamesPlayedLabel, 0);
    addStatRow(m_labelGamesWon, m_gamesWonLabel, 1);
    addStatRow(m_labelCardsLoss, m_cardsAtLossLabel, 2);
    addStatRow(m_labelTotalTime, m_totalTimeLabel, 3);
    addStatRow(m_labelPerfScore, m_performanceScoreLabel, 4);

    cardLayout->addLayout(statsGrid);
    mainLayout->addWidget(profileCard, 0, Qt::AlignCenter);
    mainLayout->addStretch();
}

void ProfileWidget::updateStats(const QJsonObject& json) {
    m_gamesPlayedLabel->setText(QString::number(json[m_keyGamesPlayed].toInt()));
    m_gamesWonLabel->setText(QString::number(json[m_keyGamesWon].toInt()));
    m_totalTimeLabel->setText(QString::number(json[m_keyHoursPlayed].toInt()));
    m_cardsAtLossLabel->setText(QString::number(json[m_keyCardsLeft].toInt()));

    m_performanceScoreLabel->setText(QString::number(json[m_keyPerfScore].toDouble(), 'f', m_precision));
}