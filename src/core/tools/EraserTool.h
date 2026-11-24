#pragma once
#include "ToolBase.h"

class EraserTool : public ToolBase {
public:
    explicit EraserTool(QObject *parent = nullptr);
    void onPress(const QPointF &pos, StrokeLayer *layer) override;
    void onMove(const QPointF &pos, StrokeLayer *layer) override;
    void onRelease(const QPointF &pos, StrokeLayer *layer) override;
    void finishStroke() override;
    void drawOverlay(QPainter &painter) const override;

private:
    void eraseAt(const QPointF &pos, StrokeLayer *layer);

    int m_radius = 20;
    bool m_active = false;
    QPointF m_cursorPos;
};