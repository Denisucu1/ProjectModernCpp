#include "CustomCard.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>

CustomCard::CustomCard(QWidget* parent) : QWidget(parent),
m_value(0), m_type(HAND_CARD), m_selected(false), m_faceDown(false) {
    setFixedSize(80, 120);
}

void CustomCard::setValue(int value) { m_value = value; update(); }
int CustomCard::getValue() const { return m_value; }
void CustomCard::setType(CardType type) { m_type = type; update(); }
CustomCard::CardType CustomCard::getType() const { return m_type; }
void CustomCard::setSelected(bool selected) { m_selected = selected; update(); }
void CustomCard::setFaceDown(bool faceDown) { m_faceDown = faceDown; update(); }

void CustomCard::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = this->rect().adjusted(2, 2, -2, -2);

    QColor bgColor = m_faceDown ? QColor("#1A237E") : Qt::white;
    if (m_selected) bgColor = QColor("#FFF176");

    painter.setBrush(bgColor);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRoundedRect(rect, 10, 10);

    if (!m_faceDown) {
        painter.setFont(QFont("Arial", 18, QFont::Bold));
        painter.setPen(Qt::black);
        painter.drawText(rect, Qt::AlignCenter, QString::number(m_value));

        painter.setFont(QFont("Arial", 8, QFont::Bold));
        if (m_type == ASCENDING) {
            painter.setPen(QColor("#2E7D32"));
            painter.drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft, "▲ 1 -> 100");
        }
        else if (m_type == DESCENDING) {
            painter.setPen(Qt::red);
            painter.drawText(rect.adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft, "▼ 100 -> 1");
        }
    }
}

void CustomCard::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) emit cardClicked(this);
}