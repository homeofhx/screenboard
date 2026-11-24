#include "FloatingButton.h"
#include <QPainter>
#include <QMouseEvent>
#include <QScreen>
#include "ui/ScreenUtils.h"
#ifdef Q_OS_MACOS
#include "platform/macos/MacWindowUtils.h"
#endif

static constexpr int BTN_SIZE = 56;
static constexpr int MARGIN = 24;

FloatingButton::FloatingButton(QWidget *parent) : QWidget(parent), m_renderer(QString(":/icons/Pen.svg")) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFocusPolicy(Qt::NoFocus);

    QSize svgSize = m_renderer.defaultSize();
    if (!svgSize.isEmpty()) {
        svgSize.scale(BTN_SIZE, BTN_SIZE, Qt::KeepAspectRatio);
    } else {
        svgSize = QSize(BTN_SIZE, BTN_SIZE);
    }

    setFixedSize(svgSize);
    setCursor(Qt::PointingHandCursor);
}

void FloatingButton::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    if (!m_positioned) {
        anchorToBottomRight();
        m_positioned = true;
    } else {
        move(ScreenUtils::keepOnScreen(pos(), size(), ScreenUtils::screenAt(frameGeometry().center())));
    }

#ifdef Q_OS_MACOS
    MacWindowUtils::showFloatingWindow(this);
#endif
}

void FloatingButton::anchorToBottomRight() {
    auto *target = ScreenUtils::screenAt(QCursor::pos());
    if (!target) return;
    const QRect screen = target->availableGeometry();
    move(screen.right() - width() - MARGIN, screen.bottom() - height() - MARGIN);
}

void FloatingButton::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    if (m_pressed) {
        const qreal scale = 0.88;
        const qreal dx = width() * (1.0 - scale) / 2.0;
        const qreal dy = height() * (1.0 - scale) / 2.0;
        p.translate(dx, dy);
        p.scale(scale, scale);
    }

    m_renderer.render(&p, QRectF(0, 0, width(), height()));
}

void FloatingButton::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;
    m_pressed = true;
    m_dragging = false;
    m_dragOrigin = event->globalPos();
    m_windowOriginOnDrag = pos();
    update();
}

void FloatingButton::mouseMoveEvent(QMouseEvent *event) {
    if (!m_pressed) return;
    const QPoint delta = event->globalPos() - m_dragOrigin;
    if (!m_dragging && delta.manhattanLength() > 6) m_dragging = true;
    if (m_dragging) move(m_windowOriginOnDrag + delta);
}

void FloatingButton::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;
    const bool wasDragging = m_dragging;
    m_pressed = false;
    m_dragging = false;
    if (wasDragging) move(ScreenUtils::keepOnScreen(pos(), size(), ScreenUtils::screenAt(frameGeometry().center())));
    update();
    if (!wasDragging) emit clicked();
}