#ifndef CUSTOMCARD_H
#define CUSTOMCARD_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class CustomCard : public QWidget
{
    Q_OBJECT

public:
    enum Type { ASCENDING, DESCENDING, DRAW_PILE, HAND_CARD };

    explicit CustomCard(QWidget* parent = nullptr);

    void setValue(int value);
    int getValue() const { return m_value; }

    void setType(Type type);
    Type getType() const { return m_type; }

    void setFaceDown(bool faceDown);
    bool isFaceDown() const { return m_faceDown; }

    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

signals:
    void cardClicked(CustomCard* card);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    int m_value;
    bool m_faceDown;
    bool m_selected;
    Type m_type;
};

#endif 