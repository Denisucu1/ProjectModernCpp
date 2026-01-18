#include "CustomCard.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>

CustomCard::CustomCard(QWidget* parent) : QWidget(parent),
m_value(m_emptyValue), m_type(HAND_CARD), m_selected(false), m_faceDown(false) {
    setFixedSize(m_cardWidth, m_cardHeight);
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

    QRect rect = this->rect().adjusted(m_rectAdjustment, m_rectAdjustment, -m_rectAdjustment, -m_rectAdjustment);

    QColor bgColor = m_faceDown ? QColor(m_colorFaceDown) : m_colorBackground;
    if (m_selected) {
        bgColor = m_colorSelected;
    }

    painter.setBrush(bgColor);
    painter.setPen(QPen(m_colorText, m_borderThickness));
    painter.drawRoundedRect(rect, m_borderRadius, m_borderRadius);

    if (!m_faceDown) {
        painter.setFont(QFont(m_fontFamily, m_mainFontSize, QFont::Bold));
        painter.setPen(m_colorText);
        painter.drawText(rect, Qt::AlignCenter, QString::number(m_value));

        painter.setFont(QFont(m_fontFamily, m_indicatorFontSize, QFont::Bold));
        QRect textRect = rect.adjusted(m_textMargin, m_textMargin, -m_textMargin, -m_textMargin);

        if (m_type == ASCENDING) {
            painter.setPen(m_colorAscending);
            painter.drawText(textRect, Qt::AlignTop | Qt::AlignLeft,
                m_txtAsc.arg(m_minValue).arg(m_maxValue));
        }
        else if (m_type == DESCENDING) {
            painter.setPen(QColor(m_colorDescending));
            painter.drawText(textRect, Qt::AlignTop | Qt::AlignLeft,
                m_txtDesc.arg(m_maxValue).arg(m_minValue));
        }
    }
}

void CustomCard::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit cardClicked(this);
    }
}