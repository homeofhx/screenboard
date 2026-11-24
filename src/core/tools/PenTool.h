#pragma once
#include "ToolBase.h"
#include <QColor>

class PenTool : public ToolBase {
public:
    explicit PenTool(QObject *parent = nullptr);
    void setColor(const QColor &c) { m_color = c; }
    void setSize(int s) { m_size = s; }
    void onPress(const QPointF &pos, StrokeLayer *layer) override;
    void onMove(const QPointF &pos, StrokeLayer *layer) override;
    void onRelease(const QPointF &pos, StrokeLayer *layer) override;
    void finishStroke() override;

private:
    QColor m_color = Qt::red;
    int m_size = 3;
    qint64 m_strokeId = -1;
};