#include "customcard.h"
#include <QPainter>
#include <QStyleOption>

CustomCard::CustomCard(QWidget* parent)
    : QWidget(parent), m_value(0), m_faceDown(false), m_selected(false), m_type(HAND_CARD)
{
    this->setFixedSize(80, 110); 
    this->setCursor(Qt::PointingHandCursor);
}

void CustomCard::setValue(int value) {
    m_value = value;
    update(); 
}

void CustomCard::setType(Type type) {
    m_type = type;
    update();
}

void CustomCard::setFaceDown(bool faceDown) {
    m_faceDown = faceDown;
    update();
}

void CustomCard::setSelected(bool selected) {
    m_selected = selected;
    update();
}

void CustomCard::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit cardClicked(this);
    }
}

void CustomCard::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect().adjusted(2, 2, -2, -2);
    QBrush background = (m_faceDown) ? QBrush(QColor("#4a4a4a")) : QBrush(Qt::white);

    painter.setBrush(background);

    QPen pen;
    pen.setWidth(m_selected ? 3 : 2);
    pen.setColor(m_selected ? Qt::yellow : Qt::black);
    painter.setPen(pen);

    painter.drawRoundedRect(rect, 10, 10);

    if (!m_faceDown) {
        painter.setPen(Qt::black);
        QFont font = painter.font();
        font.setPixelSize(24);
        font.setBold(true);
        painter.setFont(font);

        painter.drawText(rect, Qt::AlignCenter, QString::number(m_value));

        font.setPixelSize(10);
        painter.setFont(font);
        if (m_type == ASCENDING) {
            painter.setPen(QColor("darkgreen"));
            painter.drawText(rect.adjusted(0, 5, 0, 0), Qt::AlignTop | Qt::AlignHCenter, "1 -> 100");
        }
        else if (m_type == DESCENDING) {
            painter.setPen(QColor("darkred"));
            painter.drawText(rect.adjusted(0, 5, 0, 0), Qt::AlignTop | Qt::AlignHCenter, "100 -> 1");
        }
    }
    else {
        painter.setPen(Qt::white);
        painter.drawText(rect, Qt::AlignCenter, "The Game");
    }
}