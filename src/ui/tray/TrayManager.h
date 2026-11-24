#pragma once
#include <QObject>
#include <QSystemTrayIcon>

class QMenu;
class QAction;

class TrayManager : public QObject {
    Q_OBJECT

public:
    explicit TrayManager(QObject *parent = nullptr);
    ~TrayManager() override;
    void setResumeLastCaptureEnabled(bool enabled);
    bool isResumeLastCaptureEnabled() const;

signals:
    void newCaptureRequested();
    void resumeLastCaptureRequested();
    void quitRequested();

private:
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_menu;
    QAction *m_resumeLastCaptureAction;
};