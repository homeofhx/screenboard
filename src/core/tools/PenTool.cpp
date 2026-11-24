#include "PenTool.h"
#include "core/drawing/StrokeLayer.h"
#include "core/drawing/Stroke.h"

PenTool::PenTool(QObject *parent): ToolBase(parent) {}

void PenTool::onPress(const QPointF &pos, StrokeLayer *layer) {
    Stroke s;
    s.color = m_color;
    s.size = m_size;
    s.points.append(pos);
    m_strokeId = layer->addStroke(s);
}

void PenTool::onMove(const QPointF &pos, StrokeLayer *layer) {
    if (m_strokeId < 0) return;
    layer->appendPoint(m_strokeId, pos);
}

void PenTool::onRelease(const QPointF &pos, StrokeLayer *layer) {
    layer->appendPoint(m_strokeId, pos);
    finishStroke();
}

void PenTool::finishStroke() {
    m_strokeId = -1;
}