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

    const int cardWidth = 80;
    const int cardHeight = 120;
    const int borderRadius = 10;
    const int borderThickness = 2;
    const int mainFontSize = 18;
    const int indicatorFontSize = 8;
    const int rectAdjustment = 2;
    const int textMargin = 5;

    const QString colorFaceDown = "#1A237E";
    const QString colorSelected = "#FFF176";
    const QString colorAscending = "#2E7D32";
    const QString colorDescending = "red";
    const QString fontFamily = "Arial";
};

#endif