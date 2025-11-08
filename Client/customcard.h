#ifndef CUSTOMCARD_H
#define CUSTOMCARD_H

#include <QWidget>
#include <cstdint>

class CustomCard : public QWidget
{
    Q_OBJECT

public:
    explicit CustomCard(QWidget* parent = nullptr);
    void setValue(std::uint8_t value);
    std::uint8_t getValue() const;
    void setFaceDown(bool faceDown);

signals:
    void clicked(std::uint8_t value);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    std::uint8_t m_value;
    bool m_isFaceDown;
};

#endif