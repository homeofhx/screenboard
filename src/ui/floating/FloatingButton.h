#pragma once
#include <QWidget>
#include <QSvgRenderer>

class FloatingButton : public QWidget {
    Q_OBJECT

public:
    explicit FloatingButton(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void anchorToBottomRight();

    QSvgRenderer m_renderer;
    bool m_pressed = false;
    bool m_dragging = false;
    bool m_positioned = false;
    QPoint m_dragOrigin;
    QPoint m_windowOriginOnDrag;
};