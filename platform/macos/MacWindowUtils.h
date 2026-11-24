#pragma once
#include <QtGlobal>

class QWidget;

namespace MacWindowUtils {
// Keeps the launcher visible when ScreenBoard is inactive and when the user moves between desktop Spaces.
void showFloatingWindow(QWidget *widget);

// Returns zero when ScreenBoard itself is the foreground application.
qint64 frontmostExternalApplicationPid();

// Brings the previously foreground application (and its Space) back.
void activateApplication(qint64 pid);
}