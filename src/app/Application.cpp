#include "Application.h"
#include "ui/tray/TrayManager.h"
#include "ui/floating/FloatingButton.h"
#include "services/ScreenshotManager.h"
#include "ui/canvas/CanvasWindow.h"
#include "core/tools/ToolManager.h"
#include "ui/ScreenUtils.h"
#include <QIcon>
#include <QMessageBox>
#include <QScreen>
#ifdef Q_OS_MACOS
#include "platform/macos/MacWindowUtils.h"
#endif

Application::Application(int &argc, char **argv) : QApplication(argc, argv) {
    setApplicationName("ScreenBoard");
    setWindowIcon(QIcon(QStringLiteral(":/icons/AppIcon.png")));
    setQuitOnLastWindowClosed(false);

    m_toolManager = new ToolManager(this);
    m_screenshot = new ScreenshotManager(this);
    m_canvas = std::make_unique<CanvasWindow>(m_toolManager);
    m_floatingBtn = std::make_unique<FloatingButton>();
    m_tray = new TrayManager(this);

    m_captureTimer.setSingleShot(true);
    m_captureTimer.setInterval(120);
    connect(&m_captureTimer, &QTimer::timeout, this, [this] {
        const QPixmap shot = m_screenshot->capture(m_captureScreen);
        if (shot.isNull()) {
            m_floatingBtn->show();
            QMessageBox::warning(m_floatingBtn.get(), tr("New capture"),
                                 tr("Unable to capture this screen. Check screen recording permissions in the OS and try again."));
            return;
        }
        m_canvas->clearStrokes();
        m_canvas->setBackground(shot);
        m_canvas->showOnScreen(m_captureScreen);
#ifdef Q_OS_MACOS
        m_canvas->raise();
        m_canvas->activateWindow();
#endif
        m_tray->setResumeLastCaptureEnabled(true);
    });

    connect(m_floatingBtn.get(), &FloatingButton::clicked, this, &Application::onFloatingButtonClicked);
    connect(m_canvas.get(), &CanvasWindow::minimizeRequested, this, &Application::onMinimizeRequested);
    connect(m_tray, &TrayManager::newCaptureRequested, this, &Application::onFloatingButtonClicked);
    connect(m_tray, &TrayManager::resumeLastCaptureRequested, this, &Application::onResumeLastCaptureRequested);
    connect(m_tray, &TrayManager::quitRequested, this, &Application::quit);

#ifdef Q_OS_MACOS
    m_previousApplicationPid = MacWindowUtils::frontmostExternalApplicationPid();
#endif
    m_floatingBtn->show();
}

Application::~Application() = default;

void Application::onFloatingButtonClicked() {
    if (m_captureTimer.isActive()) return;

#ifdef Q_OS_MACOS
    const qint64 frontmostPid = MacWindowUtils::frontmostExternalApplicationPid();
    if (frontmostPid > 0) m_previousApplicationPid = frontmostPid;
#endif
    m_captureScreen = ScreenUtils::screenAt(m_floatingBtn->frameGeometry().center());
    m_canvas->hide();
    m_floatingBtn->hide();
    m_captureTimer.start();
}

void Application::onResumeLastCaptureRequested() {
    if (m_captureTimer.isActive()) m_captureTimer.stop();
    if (!m_tray->isResumeLastCaptureEnabled()) return;

#ifdef Q_OS_MACOS
    const qint64 frontmostPid = MacWindowUtils::frontmostExternalApplicationPid();
    if (frontmostPid > 0) m_previousApplicationPid = frontmostPid;
#endif
    m_floatingBtn->hide();
    QScreen *target = m_captureScreen.data();
    if (!target) target = m_canvas->screen();
    if (!target) target = primaryScreen();
    m_canvas->showOnScreen(target);
    m_canvas->raise();
    m_canvas->activateWindow();
}

void Application::onMinimizeRequested() {
    m_canvas->hide();
    m_captureTimer.stop();
    m_floatingBtn->show();
    m_floatingBtn->raise();
#ifdef Q_OS_MACOS
    MacWindowUtils::showFloatingWindow(m_floatingBtn.get());
    MacWindowUtils::activateApplication(m_previousApplicationPid);
#endif
}