#pragma once
#include <QWidget>
#include <QPixmap>
#include <QImage>

class ToolManager;
class StrokeLayer;
class QPainter;

class CanvasWidget : public QWidget {
public:
    explicit CanvasWidget(ToolManager *toolManager, QWidget *parent = nullptr);
    void setBackground(const QPixmap &bg);
    void clearStrokes();
    QImage contentImage() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void paintContent(QPainter &painter) const;
    void updateToolCursor();

    ToolManager *m_toolManager;
    StrokeLayer *m_strokeLayer;
    QPixmap m_background;
    bool m_drawing = false;
};