#pragma once
#include "ToolBase.h"
#include <QColor>
#include <QTimer>
#include <QPointer>
#include "core/drawing/StrokeLayer.h"

class LaserPenTool : public ToolBase {
public:
    explicit LaserPenTool(QObject *parent = nullptr);
    void setSize(int s) { m_size = s; }
    void onPress(const QPointF &pos, StrokeLayer *layer) override;
    void onMove(const QPointF &pos, StrokeLayer *layer) override;
    void onRelease(const QPointF &pos, StrokeLayer *layer) override;
    void finishStroke() override;
    void reset() override;

private:
    void onFadeTick();
    static constexpr int FADE_INTERVAL_MS = 30;
    static constexpr int FADE_STEPS = 20;

    int m_size = 4;
    qint64 m_strokeId = -1;
    int m_fadeStep = 0;
    QTimer m_fadeTimer;
    QPointer<StrokeLayer> m_layerRef;
    bool m_drawing = false;
    const QColor LASER_COLOR = QColor(255, 50, 50, 220);
};