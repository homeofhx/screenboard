#pragma once
#include <QWidget>
#include <QPixmap>
#include <memory>

class ToolManager;
class CanvasWidget;
class ToolBar;
class QScreen;

class CanvasWindow : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWindow(ToolManager *toolManager, QWidget *parent = nullptr);
    ~CanvasWindow() override;
    void setBackground(const QPixmap &bg);
    void clearStrokes();
    void showOnScreen(QScreen *screen);

signals:
    void minimizeRequested();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void saveContent();
    void setupShortcuts();

    ToolManager *m_toolManager;
    CanvasWidget *m_canvasWidget = nullptr;
    std::unique_ptr<ToolBar> m_toolBar;
};