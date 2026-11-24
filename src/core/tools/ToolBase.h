#pragma once
#include <QObject>
#include <QPointF>

class QPainter;
class StrokeLayer;

class ToolBase : public QObject {
public:
    explicit ToolBase(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~ToolBase() = default;
    virtual void onPress(const QPointF &pos, StrokeLayer *layer) = 0;
    virtual void onMove(const QPointF &pos, StrokeLayer *layer) = 0;
    virtual void onRelease(const QPointF &pos, StrokeLayer *layer) = 0;
    virtual void finishStroke() = 0;
    virtual void reset() { finishStroke(); }
    virtual void drawOverlay(QPainter &painter) const { Q_UNUSED(painter) }
};