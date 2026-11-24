#include "LaserPenTool.h"
#include "core/drawing/StrokeLayer.h"
#include "core/drawing/Stroke.h"

LaserPenTool::LaserPenTool(QObject *parent): ToolBase(parent) {
    m_fadeTimer.setInterval(FADE_INTERVAL_MS);
    connect(&m_fadeTimer, &QTimer::timeout, this, &LaserPenTool::onFadeTick);
}

void LaserPenTool::onPress(const QPointF &pos, StrokeLayer *layer) {
    if (m_layerRef) m_layerRef->setOpacity(m_strokeId, 0.0);
    reset();

    Stroke s;
    s.color = LASER_COLOR;
    s.size = m_size;
    s.opacity = 1.0;
    s.points.append(pos);
    m_strokeId = layer->addStroke(s);
    m_layerRef = layer;
    m_drawing = true;
}

void LaserPenTool::onMove(const QPointF &pos, StrokeLayer *layer) {
    if (!m_drawing || m_layerRef != layer) return;
    layer->appendPoint(m_strokeId, pos);
}

void LaserPenTool::onRelease(const QPointF &pos, StrokeLayer *layer) {
    if (!m_drawing || m_layerRef != layer) return;
    layer->appendPoint(m_strokeId, pos);
    finishStroke();
}

void LaserPenTool::finishStroke() {
    if (!m_drawing) return;
    m_drawing = false;
    m_fadeStep = 0;
    m_fadeTimer.start();
}

void LaserPenTool::reset() {
    m_fadeTimer.stop();
    m_strokeId = -1;
    m_layerRef.clear();
    m_drawing = false;
}

void LaserPenTool::onFadeTick() {
    if (!m_layerRef || !m_layerRef->containsStroke(m_strokeId)) {
        reset();
        return;
    }

    ++m_fadeStep;
    const qreal opacity = 1.0 - static_cast<qreal>(m_fadeStep) / FADE_STEPS;
    m_layerRef->setOpacity(m_strokeId, qMax(0.0, opacity));

    if (m_fadeStep >= FADE_STEPS) reset();
}