#pragma once
#include <QWidget>
#include <QColor>
#include <QVector>

class ColorPickerPopup : public QWidget {
    Q_OBJECT

public:
    static constexpr int PAD = 12;
    static constexpr int COLS = 8;
    static constexpr int ROWS = 3;
    static constexpr int SW = 22;
    static constexpr int GAP = 5;
    static constexpr int STRIP_H = 14;
    static constexpr int PREVIEW_SZ = 28;
    static constexpr int SECTION_GAP= 10;

    explicit ColorPickerPopup(QWidget *parent = nullptr);
    void setCurrentColor(const QColor &color);
    void showAbove(QWidget *anchor);

signals:
    void colorSelected(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    QRect swatchRect(int row, int col) const;
    QRect hueRect() const;
    QRect brightnessRect() const;
    QRect previewRect() const;

    void pickColor(const QPoint &pos, bool commit);

    QVector<QColor> m_presets;
    QColor m_current;
    int m_hue = 0;
    int m_value = 255;
};