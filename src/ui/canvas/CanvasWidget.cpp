#include "CanvasWidget.h"
#include "core/tools/ToolManager.h"
#include "core/tools/ToolBase.h"
#include "core/drawing/StrokeLayer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>
#include <QShowEvent>
#include <QSvgRenderer>

namespace {
constexpr int PEN_CURSOR_SIZE = 16;
constexpr int PEN_CURSOR_HOTSPOT_X = 0;
constexpr int PEN_CURSOR_HOTSPOT_Y = PEN_CURSOR_SIZE - 1;

QCursor makePenCursor(const QColor &color, qreal devicePixelRatio) {
    const QSize pixelSize(qRound(PEN_CURSOR_SIZE * devicePixelRatio), qRound(PEN_CURSOR_SIZE * devicePixelRatio));
    QPixmap pixmap(pixelSize);
    pixmap.setDevicePixelRatio(devicePixelRatio);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QSvgRenderer renderer(QStringLiteral(":/icons/PenCursor.svg"));
    renderer.render(&painter, QRectF(0, 0, PEN_CURSOR_SIZE, PEN_CURSOR_SIZE));
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(QRectF(0, 0, PEN_CURSOR_SIZE, PEN_CURSOR_SIZE), color);
    painter.end();

    return QCursor(pixmap, PEN_CURSOR_HOTSPOT_X, PEN_CURSOR_HOTSPOT_Y);
}
}

CanvasWidget::CanvasWidget(ToolManager *toolManager, QWidget *parent) : QWidget(parent), m_toolManager(toolManager) {
    m_strokeLayer = new StrokeLayer(this);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setMouseTracking(true);
    updateToolCursor();

    connect(m_strokeLayer, &StrokeLayer::changed, this, [this] { update(); });
    connect(toolManager, &ToolManager::activeToolChanged, this, [this] {
        m_drawing = false;
        m_toolManager->currentTool()->onMove(mapFromGlobal(QCursor::pos()), m_strokeLayer);
        updateToolCursor();
        update();
    });
    connect(toolManager, &ToolManager::colorChanged, this, [this] {
        updateToolCursor();
        update();
    });
    connect(toolManager, &ToolManager::sizeChanged, this, [this] { update(); });
}

void CanvasWidget::updateToolCursor() {
    switch (m_toolManager->activeTool()) {
    case ActiveTool::Pen:
        setCursor(makePenCursor(m_toolManager->color(), devicePixelRatioF()));
        break;
    case ActiveTool::LaserPen:
        setCursor(Qt::CrossCursor);
        break;
    case ActiveTool::Eraser:
        setCursor(Qt::BlankCursor);
        break;
    }
}

void CanvasWidget::setBackground(const QPixmap &bg) {
    m_background = bg;
    update();
}

void CanvasWidget::clearStrokes() {
    m_toolManager->resetTools();
    m_drawing = false;
    m_strokeLayer->clear();
}

QImage CanvasWidget::contentImage() const {
    const qreal ratio = devicePixelRatioF();
    QImage image(size() * ratio, QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(ratio);
    QPainter painter(&image);
    paintContent(painter);
    return image;
}

void CanvasWidget::paintContent(QPainter &painter) const {
    painter.fillRect(rect(), Qt::white);
    if (!m_background.isNull()) painter.drawPixmap(0, 0, width(), height(), m_background);
    m_strokeLayer->render(painter);
}

void CanvasWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    paintContent(p);
    if (underMouse()) m_toolManager->currentTool()->drawOverlay(p);
}

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;
    m_drawing = true;
    m_toolManager->currentTool()->onPress(event->localPos(), m_strokeLayer);
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    m_toolManager->currentTool()->onMove(event->localPos(), m_strokeLayer);
    update();
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton || !m_drawing) return;
    m_drawing = false;
    m_toolManager->currentTool()->onRelease(event->localPos(), m_strokeLayer);
}

void CanvasWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    updateToolCursor();
}

void CanvasWidget::hideEvent(QHideEvent *event) {
    m_toolManager->currentTool()->finishStroke();
    m_drawing = false;
    QWidget::hideEvent(event);
}

void CanvasWidget::leaveEvent(QEvent *event) {
    update();
    QWidget::leaveEvent(event);
}