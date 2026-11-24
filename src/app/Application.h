#pragma once
#include <QApplication>
#include <QTimer>
#include <QPointer>
#include <memory>

class TrayManager;
class FloatingButton;
class ScreenshotManager;
class CanvasWindow;
class ToolManager;
class QScreen;

class Application : public QApplication {
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    ~Application() override;

private:
    void onFloatingButtonClicked();
    void onResumeLastCaptureRequested();
    void onMinimizeRequested();

private:
    TrayManager *m_tray = nullptr;
    std::unique_ptr<FloatingButton> m_floatingBtn;
    ScreenshotManager *m_screenshot = nullptr;
    std::unique_ptr<CanvasWindow> m_canvas;
    ToolManager *m_toolManager = nullptr;
    QTimer m_captureTimer;
    QPointer<QScreen> m_captureScreen;
#ifdef Q_OS_MACOS
    qint64 m_previousApplicationPid = 0;
#endif
};