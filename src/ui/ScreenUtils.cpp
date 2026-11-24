#include "ScreenUtils.h"
#include <QGuiApplication>
#include <QScreen>

QScreen *ScreenUtils::screenAt(const QPoint &position) {
    auto *screen = QGuiApplication::screenAt(position);
    return screen ? screen : QGuiApplication::primaryScreen();
}

QPoint ScreenUtils::keepOnScreen(const QPoint &position, const QSize &size, QScreen *screen) {
    if (!screen) return position;
    const QRect available = screen->availableGeometry();
    return {qBound(available.left(), position.x(), qMax(available.left(), available.right() - size.width() + 1)),
            qBound(available.top(), position.y(), qMax(available.top(), available.bottom() - size.height() + 1))};
}