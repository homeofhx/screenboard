#include "ScreenshotManager.h"
#include <QScreen>

ScreenshotManager::ScreenshotManager(QObject *parent) : QObject(parent) {}

QPixmap ScreenshotManager::capture(QScreen *screen) const {
    if (!screen) return {};
    return screen->grabWindow(0);
}