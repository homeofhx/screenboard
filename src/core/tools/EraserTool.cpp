#include "EraserTool.h"
#include "core/drawing/StrokeLayer.h"
#include <QPainter>

EraserTool::EraserTool(QObject *parent): ToolBase(parent) {}

void EraserTool::onPress(const QPointF &pos, StrokeLayer *layer) {
    m_active = true;
    m_cursorPos = pos;
    eraseAt(pos, layer);
}

void EraserTool::onMove(const QPointF &pos, StrokeLayer *layer) {
    if (m_active) layer->eraseAlong(m_cursorPos, pos, m_radius);
    m_cursorPos = pos;
}

void EraserTool::onRelease(const QPointF &pos, StrokeLayer *layer) {
    onMove(pos, layer);
    finishStroke();
}

void EraserTool::finishStroke() {
    m_active = false;
}

void EraserTool::drawOverlay(QPainter &painter) const {
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(255, 255, 255, 180), 2, Qt::SolidLine));
    painter.drawEllipse(m_cursorPos, m_radius, m_radius);
    painter.setPen(QPen(QColor(255, 80, 80, 200), 1.5));
    painter.drawEllipse(m_cursorPos, 2, 2);
    painter.restore();
}

void EraserTool::eraseAt(const QPointF &pos, StrokeLayer *layer) {
    layer->eraseNear(pos, m_radius);
}