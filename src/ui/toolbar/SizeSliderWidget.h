#pragma once
#include <QWidget>

class ToolManager;
class QSlider;
class QLabel;

class SizeSliderWidget : public QWidget {
public:
    static constexpr int BRIDGE_H = 10;
    explicit SizeSliderWidget(ToolManager *toolManager, QWidget *parent = nullptr);
    void showAboveToolbar(QWidget *toolbar);

private:
    void onSliderMoved(int value);
    void onSizeChanged(int size);

private:
    ToolManager *m_toolManager;
    QSlider *m_slider;
    QLabel *m_label;

protected:
    void paintEvent(QPaintEvent *) override;
};