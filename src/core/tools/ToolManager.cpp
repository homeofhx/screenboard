#include "ToolManager.h"
#include "PenTool.h"
#include "LaserPenTool.h"
#include "EraserTool.h"

ToolManager::ToolManager(QObject *parent): QObject(parent) {
    m_pen = new PenTool(this);
    m_laser = new LaserPenTool(this);
    m_eraser = new EraserTool(this);
    m_pen->setColor(m_color);
    m_pen->setSize(m_size);
    m_laser->setSize(m_size);
}

void ToolManager::setActiveTool(ActiveTool tool) {
    if (m_activeTool == tool) return;
    currentTool()->finishStroke();
    m_activeTool = tool;
    emit activeToolChanged(tool);
}

void ToolManager::setColor(const QColor &color) {
    if (!color.isValid() || m_color == color) return;
    m_color = color;
    m_pen->setColor(color);
    emit colorChanged(color);
}

void ToolManager::setSize(int size) {
    size = qBound(1, size, 40);
    if (m_size == size) return;
    m_size = size;
    m_pen->setSize(size);
    m_laser->setSize(size);
    emit sizeChanged(size);
}

void ToolManager::resetTools() {
    m_pen->reset();
    m_laser->reset();
    m_eraser->reset();
}

ToolBase *ToolManager::currentTool() const {
    switch (m_activeTool) {
    case ActiveTool::LaserPen: return m_laser;
    case ActiveTool::Eraser: return m_eraser;
    default: return m_pen;
    }
}