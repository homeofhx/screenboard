#include "TrayManager.h"
#include <QMenu>
#include <QAction>

TrayManager::TrayManager(QObject *parent) : QObject(parent) {
    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/TrayIcon.svg"), this);
    m_trayIcon->setToolTip("ScreenBoard");

    m_menu = new QMenu();

    QAction *newCaptureAct = m_menu->addAction("New Capture");
    m_resumeLastCaptureAction = m_menu->addAction("Resume Last Capture");
    QAction *quitAct = m_menu->addAction("Quit!");
    m_resumeLastCaptureAction->setEnabled(false);

    connect(newCaptureAct, &QAction::triggered, this, &TrayManager::newCaptureRequested);
    connect(m_resumeLastCaptureAction, &QAction::triggered, this, &TrayManager::resumeLastCaptureRequested);
    connect(quitAct, &QAction::triggered, this, &TrayManager::quitRequested);

    m_trayIcon->setContextMenu(m_menu);
    m_trayIcon->show();
}

void TrayManager::setResumeLastCaptureEnabled(bool enabled) {
    m_resumeLastCaptureAction->setEnabled(enabled);
}

bool TrayManager::isResumeLastCaptureEnabled() const {
    return m_resumeLastCaptureAction->isEnabled();
}

TrayManager::~TrayManager() {
    m_trayIcon->setContextMenu(nullptr);
    delete m_menu;
}