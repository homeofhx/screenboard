#include "ToolBar.h"
#include "ColorPickerPopup.h"
#include "SizeSliderWidget.h"
#include "core/tools/ToolManager.h"
#include "ui/ScreenUtils.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QPainter>
#include <QScreen>

static const QString BTN_STYLE = R"(
    QToolButton {
        background: transparent;
        border: none;
        padding: 1px;
        color: white;
        font-size: 18px;
    }
    QToolButton:checked {
        background: rgba(255, 255, 255, 35);
        border-radius: 8px;
    }
)";

ToolBar::ToolBar(ToolManager *toolManager, QWidget *parent): QWidget(parent), m_toolManager(toolManager) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    m_colorPopup = new ColorPickerPopup(this);
    m_sizePopup = new SizeSliderWidget(toolManager, this);
    setupLayout();
    connectSignals();
}

void ToolBar::setupLayout() {
    m_penBtn = makeIconButton(":/icons/PenIdle.svg", "Pen (P)");
    m_laserBtn = makeIconButton(":/icons/TraceIdle.svg", "Laser pen (L)");
    m_eraserBtn = makeIconButton(":/icons/EraserIdle.svg", "Eraser (E)");
    m_colorBtn = makeIconButton(":/icons/Color.svg", "Pen color");
    m_sizeBtn = makeIconButton(":/icons/Size.svg", "Pen size");
    m_clearBtn = makeIconButton(":/icons/CLRALL.svg", "Clear all strokes");
    m_saveBtn = makeIconButton(":/icons/Save.svg", "Save as PNG");
    m_minimizeBtn = makeIconButton(":/icons/Minimize.svg", "Minimize");
    for (auto *b : {m_penBtn, m_laserBtn, m_eraserBtn}) b->setCheckable(true);
    updateToolSelection(m_toolManager->activeTool());

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 8, 16, 8);
    layout->addWidget(m_penBtn);
    layout->addWidget(m_laserBtn);
    layout->addWidget(m_eraserBtn);
    layout->addWidget(m_colorBtn);
    layout->addWidget(m_sizeBtn);
    layout->addWidget(m_clearBtn);
    layout->addWidget(m_saveBtn);
    layout->addSpacing(10);
    layout->addWidget(m_minimizeBtn);

    adjustSize();
}

void ToolBar::positionOnScreen(QScreen *screen) {
    if (!screen) return;
    adjustSize();
    const QRect available = screen->availableGeometry();
    move(ScreenUtils::keepOnScreen({available.center().x() - width() / 2,
                                    available.bottom() - height() - 16}, size(), screen));
}

void ToolBar::updateToolSelection(ActiveTool tool) {
    m_penBtn->setChecked(tool == ActiveTool::Pen);
    m_laserBtn->setChecked(tool == ActiveTool::LaserPen);
    m_eraserBtn->setChecked(tool == ActiveTool::Eraser);
    m_penBtn->setIcon(QIcon(tool == ActiveTool::Pen ? ":/icons/Pen.svg" : ":/icons/PenIdle.svg"));
    m_laserBtn->setIcon(QIcon(tool == ActiveTool::LaserPen ? ":/icons/Trace.svg" : ":/icons/TraceIdle.svg"));
    m_eraserBtn->setIcon(QIcon(tool == ActiveTool::Eraser ? ":/icons/Eraser.svg" : ":/icons/EraserIdle.svg"));
}

void ToolBar::connectSignals() {
    connect(m_toolManager, &ToolManager::activeToolChanged, this, &ToolBar::updateToolSelection);
    connect(m_penBtn, &QToolButton::clicked, this, [this] {
        m_toolManager->setActiveTool(ActiveTool::Pen);
    });
    connect(m_laserBtn, &QToolButton::clicked, this, [this] {
        m_toolManager->setActiveTool(ActiveTool::LaserPen);
    });
    connect(m_eraserBtn, &QToolButton::clicked, this, [this] {
        m_toolManager->setActiveTool(ActiveTool::Eraser);
    });
    connect(m_colorBtn, &QToolButton::clicked, this, [this] {
        m_colorPopup->setCurrentColor(m_toolManager->color());
        m_colorPopup->showAbove(m_colorBtn);
    });
    connect(m_colorPopup, &ColorPickerPopup::colorSelected, m_toolManager, &ToolManager::setColor);
    connect(m_sizeBtn, &QToolButton::clicked, this, [this] { m_sizePopup->showAboveToolbar(this); });
    connect(m_clearBtn, &QToolButton::clicked, this, &ToolBar::clearRequested);
    connect(m_saveBtn, &QToolButton::clicked, this, &ToolBar::saveRequested);
    connect(m_minimizeBtn, &QToolButton::clicked, this, &ToolBar::minimizeRequested);
}

QToolButton *ToolBar::makeIconButton(const QString &iconPath, const QString &tooltip) {
    auto *btn = new QToolButton(this);
    btn->setIcon(QIcon(iconPath));
    btn->setIconSize(QSize(35, 35));
    btn->setToolTip(tooltip);
    btn->setStyleSheet(BTN_STYLE);
    btn->setFixedSize(38, 38);
    return btn;
}

void ToolBar::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(20, 20, 20, 215));
    p.drawRoundedRect(rect(), 18, 18);
    p.setPen(QPen(QColor(255, 255, 255, 25), 1));
    p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 17, 17);
}