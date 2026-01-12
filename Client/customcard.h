#ifndef CUSTOMCARD_H
#define CUSTOMCARD_H

#include <QWidget>

class CustomCard : public QWidget {
    Q_OBJECT
public:
    enum CardType { HAND_CARD, ASCENDING, DESCENDING, DRAW_PILE };

    explicit CustomCard(QWidget* parent = nullptr);

    void setValue(int value);
    int getValue() const;
    void setType(CardType type);
    CardType getType() const;
    void setSelected(bool selected);
    void setFaceDown(bool faceDown);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void cardClicked(CustomCard* card);

private:
    int m_value;
    CardType m_type;
    bool m_selected;
    bool m_faceDown;
};

#endif