#include "CustomCard.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>

CustomCard::CustomCard(QWidget* parent) : QWidget(parent),
m_value(0), m_type(HAND_CARD), m_selected(false), m_faceDown(false) {
    setFixedSize(cardWidth, cardHeight);
}

void CustomCard::setValue(int value) {
    m_value = value;
    update();
}

int CustomCard::getValue() const {
    return m_value;
}

void CustomCard::setType(CardType type) {
    m_type = type;
    update();
}

CustomCard::CardType CustomCard::getType() const {
    return m_type;
}

void CustomCard::setSelected(bool selected) {
    m_selected = selected;
    update();
}

void CustomCard::setFaceDown(bool faceDown) {
    m_faceDown = faceDown;
    update();
}

void CustomCard::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect().adjusted(rectAdjustment, rectAdjustment, -rectAdjustment, -rectAdjustment);
    QColor bgColor = m_faceDown ? QColor(colorFaceDown) : Qt::white;
    if (m_selected) {
        bgColor = QColor(colorSelected);
    }

    painter.setBrush(bgColor);
    painter.setPen(QPen(Qt::black, borderThickness));
    painter.drawRoundedRect(rect, borderRadius, borderRadius);

    if (!m_faceDown) {
        painter.setFont(QFont(fontFamily, mainFontSize, QFont::Bold));
        painter.setPen(Qt::black);
        painter.drawText(rect, Qt::AlignCenter, QString::number(m_value));

        painter.setFont(QFont(fontFamily, indicatorFontSize, QFont::Bold));
        QRect textRect = rect.adjusted(textMargin, textMargin, -textMargin, -textMargin);

        if (m_type == ASCENDING) {
            painter.setPen(QColor(colorAscending));
            painter.drawText(textRect, Qt::AlignTop | Qt::AlignLeft, "▲ 1 -> 100");
        }
        else if (m_type == DESCENDING) {
            painter.setPen(QColor(colorDescending));
            painter.drawText(textRect, Qt::AlignTop | Qt::AlignLeft, "▼ 100 -> 1");
        }
    }
}

void CustomCard::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit cardClicked(this);
    }
}