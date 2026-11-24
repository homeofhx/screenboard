#include "CanvasWindow.h"
#include "CanvasWidget.h"
#include "ui/toolbar/ToolBar.h"
#include "core/tools/ToolManager.h"
#include "services/ImageExporter.h"
#include <QVBoxLayout>
#include <QAction>
#include <QTimer>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QScreen>
#include <QWindow>
#include <QCloseEvent>

CanvasWindow::CanvasWindow(ToolManager *toolManager, QWidget *parent) : QWidget(parent), m_toolManager(toolManager) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose, false);

    m_canvasWidget = new CanvasWidget(toolManager, this);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_canvasWidget);

    m_toolBar = std::make_unique<ToolBar>(toolManager);

    connect(m_toolBar.get(), &ToolBar::minimizeRequested, this, &CanvasWindow::minimizeRequested);
    connect(m_toolBar.get(), &ToolBar::clearRequested, m_canvasWidget, &CanvasWidget::clearStrokes);
    connect(m_toolBar.get(), &ToolBar::saveRequested, this, &CanvasWindow::saveContent);

    setupShortcuts();
}

CanvasWindow::~CanvasWindow() = default;

void CanvasWindow::setBackground(const QPixmap &bg) {
    m_canvasWidget->setBackground(bg);
}

void CanvasWindow::clearStrokes() {
    m_canvasWidget->clearStrokes();
}

void CanvasWindow::showOnScreen(QScreen *target) {
    if (!target) return;

    winId();
    windowHandle()->setScreen(target);
#ifdef Q_OS_MACOS
    setWindowState(Qt::WindowNoState);
    setGeometry(target->geometry());
    show();
#else
    setGeometry(target->geometry());
    showFullScreen();
#endif
    m_toolBar->positionOnScreen(target);
}

void CanvasWindow::saveContent() {
    const QImage image = m_canvasWidget->contentImage();
    if (image.isNull()) {
        QMessageBox::warning(m_toolBar.get(), tr("Save whiteboard"), tr("Unable to create the whiteboard image."));
        return;
    }

    QString directory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (directory.isEmpty()) directory = QDir::homePath();

    QFileDialog dialog(m_toolBar.get(), tr("Save whiteboard"), directory);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("PNG image (*.png)"));
    dialog.setDefaultSuffix(QStringLiteral("png"));
    dialog.selectFile(ImageExporter::defaultFileName());
    dialog.setWindowModality(Qt::ApplicationModal);
    if (dialog.exec() != QDialog::Accepted || dialog.selectedFiles().isEmpty()) return;

    const QString selectedName = dialog.selectedFiles().first();
    const QString fileName = ImageExporter::pngFileName(selectedName);
    if (fileName != selectedName) {
        if (QFileInfo::exists(fileName) && QMessageBox::question(m_toolBar.get(), tr("Save whiteboard"),
                                                                 tr("%1 already exists. Replace it?").arg(fileName),
                                                                 QMessageBox::Yes | QMessageBox::No,
                                                                 QMessageBox::No) != QMessageBox::Yes)
            return;
    }

    QString error;
    if (!ImageExporter::savePng(image, fileName, &error)) {
        QMessageBox::warning(m_toolBar.get(), tr("Save whiteboard"),
                             tr("Unable to save the image to %1.\n%2").arg(fileName, error));
    }
}

void CanvasWindow::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    QTimer::singleShot(0, this, [this] {
        if (!isVisible()) return;
        m_toolBar->positionOnScreen(screen());
        m_toolBar->show();
    });
}

void CanvasWindow::hideEvent(QHideEvent *event) {
    QWidget::hideEvent(event);
    m_toolBar->hide();
}

void CanvasWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    emit minimizeRequested();
}

void CanvasWindow::setupShortcuts() {
    auto shortcut = [this](const QKeySequence &key, auto handler) {
        auto *action = new QAction(this);
        action->setShortcut(key);
        action->setShortcutContext(Qt::WindowShortcut);
        addAction(action);
        m_toolBar->addAction(action);
        connect(action, &QAction::triggered, this, handler);
    };

    shortcut(QKeySequence(Qt::Key_Escape), [this] { emit minimizeRequested(); });
    shortcut(QKeySequence(Qt::Key_P), [this] { m_toolManager->setActiveTool(ActiveTool::Pen); });
    shortcut(QKeySequence(Qt::Key_L), [this] { m_toolManager->setActiveTool(ActiveTool::LaserPen); });
    shortcut(QKeySequence(Qt::Key_E), [this] { m_toolManager->setActiveTool(ActiveTool::Eraser); });
    shortcut(QKeySequence::Save, [this] { saveContent(); });
}