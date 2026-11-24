#pragma once
#include <QObject>
#include <QVector>
#include <QHash>
#include "Stroke.h"

class QPainter;

class StrokeLayer : public QObject {
    Q_OBJECT

public:
    explicit StrokeLayer(QObject *parent = nullptr);

    // IDs are never reused, even after clear(), so delayed tool events stay safe.
    qint64 addStroke(const Stroke &stroke);

    // Append a point to an in-progress stroke
    void appendPoint(qint64 id, const QPointF &pt);

    // Erase whole strokes intersecting the eraser disk or its swept path.
    void eraseNear(const QPointF &pos, int radius);
    void eraseAlong(const QPointF &from, const QPointF &to, int radius);

    // Set the opacity on a specific stroke (for laser pen fade)
    void setOpacity(qint64 id, qreal opacity);
    bool containsStroke(qint64 id) const;

    void clear();

    // Paint all active strokes onto painter (no background — caller does that)
    void render(QPainter &painter) const;

signals:
    void changed();

private:
    QVector<Stroke> m_strokes;
    QHash<qint64, int> m_strokeIndices;
    qint64 m_nextStrokeId = 0;
};