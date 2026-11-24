#pragma once
#include <QWidget>

class ToolManager;
class ColorPickerPopup;
class SizeSliderWidget;
class QToolButton;
class QScreen;
enum class ActiveTool;

class ToolBar : public QWidget {
    Q_OBJECT

public:
    explicit ToolBar(ToolManager *toolManager, QWidget *parent = nullptr);
    void positionOnScreen(QScreen *screen);

signals:
    void minimizeRequested();
    void clearRequested();
    void saveRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupLayout();
    void connectSignals();
    void updateToolSelection(ActiveTool tool);

    QToolButton *makeIconButton(const QString &iconPath, const QString &tooltip);
    ToolManager *m_toolManager;
    ColorPickerPopup *m_colorPopup = nullptr;
    SizeSliderWidget *m_sizePopup = nullptr;
    QToolButton *m_penBtn = nullptr;
    QToolButton *m_laserBtn = nullptr;
    QToolButton *m_eraserBtn = nullptr;
    QToolButton *m_colorBtn = nullptr;
    QToolButton *m_sizeBtn = nullptr;
    QToolButton *m_clearBtn = nullptr;
    QToolButton *m_saveBtn = nullptr;
    QToolButton *m_minimizeBtn = nullptr;
};