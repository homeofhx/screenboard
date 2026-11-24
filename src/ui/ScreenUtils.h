#pragma once
#include <QPoint>
#include <QSize>

class QScreen;

namespace ScreenUtils {
QScreen *screenAt(const QPoint &position);
QPoint keepOnScreen(const QPoint &position, const QSize &size, QScreen *screen);
}