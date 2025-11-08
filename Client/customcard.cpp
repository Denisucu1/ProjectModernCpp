#include "customcard.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QMouseEvent>

CustomCard::CustomCard(QWidget* parent)
    : QWidget(parent), m_value(0)
{
    setFixedSize(80, 120);
}

void CustomCard::setValue(std::uint8_t value)
{
    m_value = value;
    update();
}

std::uint8_t CustomCard::getValue() const
{
    return m_value;
}

void CustomCard::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect cardRect = this->rect().adjusted(1, 1, -1, -1);

    QColor backgroundColor(250, 250, 245);
    QColor borderColor(100, 100, 100);
    QColor textColor(30, 30, 30);

    painter.setBrush(backgroundColor);
    painter.setPen(QPen(borderColor, 2));
    painter.drawRoundedRect(cardRect, 10, 10);

    QFont font = painter.font();
    font.setPointSize(24);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(textColor);

    painter.drawText(cardRect, Qt::AlignCenter, QString::number(m_value));
}

void CustomCard::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked(m_value);
    }
}