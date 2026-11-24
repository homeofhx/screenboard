#include "StrokeLayer.h"
#include <QPainter>
#include <QPainterPath>
#include <QLineF>
#include <algorithm>

namespace {
qreal pointSegmentDistanceSquared(const QPointF &point, const QPointF &from, const QPointF &to) {
    const QPointF segment = to - from;
    const qreal lengthSquared = QPointF::dotProduct(segment, segment);
    const qreal t = lengthSquared > 0 ? qBound(0.0, QPointF::dotProduct(point - from, segment) / lengthSquared, 1.0) : 0;
    const QPointF offset = point - (from + t * segment);
    return QPointF::dotProduct(offset, offset);
}

qreal segmentDistanceSquared(const QPointF &a, const QPointF &b, const QPointF &c, const QPointF &d) {
    if (QLineF(a, b).intersects(QLineF(c, d), nullptr) == QLineF::BoundedIntersection) return 0;
    return std::min({pointSegmentDistanceSquared(a, c, d), pointSegmentDistanceSquared(b, c, d),
                     pointSegmentDistanceSquared(c, a, b), pointSegmentDistanceSquared(d, a, b)});
}
}

StrokeLayer::StrokeLayer(QObject *parent): QObject(parent) {}

qint64 StrokeLayer::addStroke(const Stroke &stroke) {
    const qint64 id = m_nextStrokeId++;
    m_strokeIndices.insert(id, m_strokes.size());
    m_strokes.append(stroke);
    emit changed();
    return id;
}

void StrokeLayer::appendPoint(qint64 id, const QPointF &pt) {
    const auto index = m_strokeIndices.constFind(id);
    if (index == m_strokeIndices.cend()) return;
    auto &stroke = m_strokes[index.value()];
    if (!stroke.active || (!stroke.points.isEmpty() && stroke.points.last() == pt)) return;
    stroke.points.append(pt);
    emit changed();
}

void StrokeLayer::eraseNear(const QPointF &pos, int radius) {
    eraseAlong(pos, pos, radius);
}

void StrokeLayer::eraseAlong(const QPointF &from, const QPointF &to, int radius) {
    bool anyErased = false;

    for (Stroke &s : m_strokes) {
        if (!s.active || s.points.isEmpty()) continue;
        const qreal reach = qMax(0, radius) + s.size / 2.0;
        for (int i = 0; i < s.points.size(); ++i) {
            const QPointF previous = s.points[qMax(0, i - 1)];
            if (segmentDistanceSquared(from, to, previous, s.points[i]) <= reach * reach) {
                s.active = false;
                anyErased = true;
                break;   // No need to check remaining points of this stroke
            }
        }
    }

    if (anyErased) emit changed();
}

void StrokeLayer::setOpacity(qint64 id, qreal opacity) {
    const auto index = m_strokeIndices.constFind(id);
    if (index == m_strokeIndices.cend()) return;
    auto &stroke = m_strokes[index.value()];
    stroke.opacity = qBound(0.0, opacity, 1.0);
    if (stroke.opacity <= 0.0) stroke.active = false;
    emit changed();
}

bool StrokeLayer::containsStroke(qint64 id) const {
    return m_strokeIndices.contains(id);
}

void StrokeLayer::clear() {
    m_strokes.clear();
    m_strokeIndices.clear();
    emit changed();
}

void StrokeLayer::render(QPainter &painter) const {
    painter.setRenderHint(QPainter::Antialiasing);

    for (const Stroke &s : m_strokes) {
        if (!s.active || s.points.isEmpty()) continue;

        QColor c = s.color;
        c.setAlphaF(c.alphaF() * s.opacity);

        QPen pen(c, s.size, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);

        if (s.points.size() == 1) {
            painter.drawPoint(s.points.first());
        } else {
            QPainterPath path(s.points.first());
            for (int i = 1; i < s.points.size(); ++i) path.lineTo(s.points[i]);
            painter.drawPath(path);
        }
    }
}