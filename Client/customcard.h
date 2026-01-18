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

    const int m_cardWidth = 80;
    const int m_cardHeight = 120;
    const int m_borderRadius = 10;
    const int m_borderThickness = 2;
    const int m_mainFontSize = 18;
    const int m_indicatorFontSize = 8;
    const int m_rectAdjustment = 2;
    const int m_textMargin = 5;

    const QColor m_colorFaceDown = QColor("#1A237E");
    const QColor m_colorSelected = QColor("#FFF176");
    const QColor m_colorAscending = QColor("#2E7D32");
    const QColor m_colorDescending = Qt::red;
    const QColor m_colorBackground = Qt::white; 
    const QColor m_colorText = Qt::black;    
    const QString m_fontFamily = "Arial";

    const int m_minValue = 1;
    const int m_maxValue = 100;
    const int m_emptyValue = 0;

    const QString m_txtAsc = "▲ %1 -> %2";
    const QString m_txtDesc = "▼ %1 -> %2";
};
#endif