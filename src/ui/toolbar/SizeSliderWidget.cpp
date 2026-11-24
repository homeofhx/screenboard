#include "SizeSliderWidget.h"
#include "core/tools/ToolManager.h"
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include "ui/ScreenUtils.h"

SizeSliderWidget::SizeSliderWidget(ToolManager *toolManager, QWidget *parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_toolManager(toolManager) {
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(200);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(1, 40);
    m_slider->setValue(toolManager->size());
    m_slider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            height: 4px;
            background: rgba(255,255,255,40);
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            width: 14px; height: 14px;
            margin: -5px 0;
            border-radius: 7px;
            background: white;
        }
        QSlider::sub-page:horizontal {
            background: rgba(255,255,255,180);
            border-radius: 2px;
        }
    )");

    m_label = new QLabel(QString::number(toolManager->size()), this);
    m_label->setFixedWidth(28);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet("color: white; font-size: 12px; background: transparent;");

    auto *row = new QHBoxLayout;
    row->setSpacing(6);
    row->addWidget(m_slider);
    row->addWidget(m_label);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(14, 12, 14, 12 + BRIDGE_H);
    root->addLayout(row);

    connect(m_slider, &QSlider::valueChanged, this, &SizeSliderWidget::onSliderMoved);
    connect(toolManager, &ToolManager::sizeChanged, this, &SizeSliderWidget::onSizeChanged);
}

void SizeSliderWidget::showAboveToolbar(QWidget *toolbar) {
    adjustSize();
    const QPoint tg = toolbar->mapToGlobal(QPoint(0, 0));
    const int x = tg.x() + toolbar->width() / 2 - width() / 2;
    const int y = tg.y() - height() + BRIDGE_H;
    move(ScreenUtils::keepOnScreen({x, y}, size(), ScreenUtils::screenAt(toolbar->mapToGlobal(toolbar->rect().center()))));
    show();
}

void SizeSliderWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRect boxRect = rect().adjusted(0, 0, 0, -BRIDGE_H);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(20, 20, 20, 215));
    p.drawRoundedRect(boxRect, 12, 12);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(255, 255, 255, 25), 1));
    p.drawRoundedRect(boxRect.adjusted(1, 1, -1, -1), 11, 11);
}

void SizeSliderWidget::onSliderMoved(int value) {
    m_label->setText(QString::number(value));
    m_toolManager->setSize(value);
}

void SizeSliderWidget::onSizeChanged(int size) {
    m_slider->setValue(size);
    m_label->setText(QString::number(size));
}