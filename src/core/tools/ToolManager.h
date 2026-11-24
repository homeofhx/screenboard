#pragma once
#include <QObject>
#include <QColor>

class ToolBase;
class PenTool;
class LaserPenTool;
class EraserTool;

enum class ActiveTool { Pen, LaserPen, Eraser };

class ToolManager : public QObject {
    Q_OBJECT

public:
    explicit ToolManager(QObject *parent = nullptr);
    void setActiveTool(ActiveTool tool);
    void setColor(const QColor &color);
    void setSize(int size);
    void resetTools();
    ActiveTool activeTool() const { return m_activeTool; }
    QColor color() const { return m_color; }
    int size() const { return m_size; }
    ToolBase *currentTool() const;

signals:
    void activeToolChanged(ActiveTool tool);
    void colorChanged(const QColor &color);
    void sizeChanged(int size);

private:
    ActiveTool m_activeTool = ActiveTool::Pen;
    QColor m_color = Qt::red;
    int m_size = 4;
    PenTool *m_pen = nullptr;
    LaserPenTool *m_laser = nullptr;
    EraserTool *m_eraser = nullptr;
};