#pragma once
#include <QObject>
#include <QPixmap>

class QScreen;

class ScreenshotManager : public QObject {
public:
    explicit ScreenshotManager(QObject *parent = nullptr);
    QPixmap capture(QScreen *screen) const;
};