#pragma once
#include <QVector>
#include <QPointF>
#include <QColor>

struct Stroke {
    QVector<QPointF> points;
    QColor color = Qt::red;
    int size = 3;
    bool active = true;
    qreal opacity = 1.0;
};