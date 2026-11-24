#include "ColorPickerPopup.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLinearGradient>
#include "ui/ScreenUtils.h"

static QVector<QColor> buildPresets() {
    return {
        "#e53935", "#e67e22", "#f1c40f", "#2ecc71",
        "#1abc9c", "#3498db", "#9b59b6", "#ffffff",
        "#c0392b", "#d35400", "#f39c12", "#27ae60",
        "#16a085", "#2980b9", "#8e44ad", "#bdc3c7",
        "#922b21", "#784212", "#9a7d0a", "#1e8449",
        "#0e6655", "#1a5276", "#6c3483", "#2c3e50",
    };
}

static constexpr int GRID_W = ColorPickerPopup::COLS * ColorPickerPopup::SW + (ColorPickerPopup::COLS - 1) * ColorPickerPopup::GAP;
static constexpr int WIDGET_W = GRID_W + 2 * ColorPickerPopup::PAD;

QRect ColorPickerPopup::swatchRect(int row, int col) const {
    const int x = PAD + col * (SW + GAP);
    const int y = PAD + row * (SW + GAP);
    return { x, y, SW, SW };
}

QRect ColorPickerPopup::hueRect() const {
    const int swatchBottom = PAD + ROWS * SW + (ROWS - 1) * GAP;
    const int y = swatchBottom + SECTION_GAP;
    return { PAD, y, GRID_W, STRIP_H };
}

QRect ColorPickerPopup::brightnessRect() const {
    return hueRect().translated(0, STRIP_H + GAP);
}

QRect ColorPickerPopup::previewRect() const {
    const int y = brightnessRect().bottom() + SECTION_GAP;
    return { PAD, y, PREVIEW_SZ, PREVIEW_SZ };
}

ColorPickerPopup::ColorPickerPopup(QWidget *parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_presets(buildPresets())
    , m_current(Qt::red) {
    setAttribute(Qt::WA_TranslucentBackground);

    const int h = previewRect().bottom() + PAD;
    setFixedSize(WIDGET_W, h);

    setStyleSheet("");
}

void ColorPickerPopup::setCurrentColor(const QColor &color) {
    m_current = color;
    m_hue = color.hsvHue() < 0 ? 0 : color.hsvHue();
    m_value = color.value();
    update();
}

void ColorPickerPopup::showAbove(QWidget *anchor) {
    const QPoint ag = anchor->mapToGlobal(QPoint(0, 0));
    int x = ag.x() + anchor->width() / 2 - width() / 2;
    int y = ag.y() - height() - 8;

    move(ScreenUtils::keepOnScreen({x, y}, size(), ScreenUtils::screenAt(anchor->mapToGlobal(anchor->rect().center()))));
    show();
}

void ColorPickerPopup::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(22, 22, 22, 220));
    p.drawRoundedRect(rect(), 14, 14);
    p.setPen(QPen(QColor(255, 255, 255, 20), 1));
    p.drawRoundedRect(rect().adjusted(1,1,-1,-1), 13, 13);
    p.setPen(Qt::NoPen);

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            const int idx = r * COLS + c;
            if (idx >= m_presets.size()) continue;
            const QColor &col = m_presets[idx];
            const QRect   sr  = swatchRect(r, c);

            if (col == m_current) {
                p.setBrush(Qt::NoBrush);
                p.setPen(QPen(Qt::white, 2));
                p.drawEllipse(sr.adjusted(-2,-2,2,2));
                p.setPen(Qt::NoPen);
            }

            p.setBrush(col);
            p.drawEllipse(sr);
        }
    }

    {
        const QRect hr = hueRect();
        QLinearGradient hg(hr.left(), 0, hr.right(), 0);
        for (int h = 0; h <= 360; h += 30) hg.setColorAt(h / 360.0, QColor::fromHsv(h % 360, 255, 255));
        p.setBrush(hg);
        p.drawRoundedRect(hr, STRIP_H / 2, STRIP_H / 2);

        const int cx = hr.left() + static_cast<int>(m_hue / 359.0 * hr.width());
        p.setBrush(Qt::white);
        p.setPen(QPen(QColor(0,0,0,120), 1));
        p.drawEllipse(QPoint(cx, hr.center().y()), 6, 6);
    }

    {
        const QRect br = brightnessRect();
        QLinearGradient bg(br.left(), 0, br.right(), 0);
        bg.setColorAt(0.0, Qt::black);
        bg.setColorAt(1.0, QColor::fromHsv(m_hue, 255, 255));
        p.setBrush(bg);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(br, STRIP_H / 2, STRIP_H / 2);

        const int cx = br.left() + static_cast<int>(m_value / 255.0 * br.width());
        p.setBrush(QColor::fromHsv(m_hue, 255, m_value));
        p.setPen(QPen(Qt::white, 1.5));
        p.drawEllipse(QPoint(cx, br.center().y()), 6, 6);
    }

    {
        const QRect pr = previewRect();
        p.setPen(Qt::NoPen);
        const int cell = 7;

        for (int gy = pr.top(); gy < pr.bottom(); gy += cell) {
            for (int gx = pr.left(); gx < pr.right(); gx += cell) {
                const bool light = ((gx / cell + gy / cell) % 2 == 0);
                p.setBrush(light ? QColor(200,200,200) : QColor(140,140,140));
                p.drawRect(QRect(gx, gy, qMin(cell, pr.right()  - gx), qMin(cell, pr.bottom() - gy)));
            }
        }

        p.setBrush(m_current);
        p.drawRoundedRect(pr, 5, 5);
        p.setPen(QPen(QColor(255,255,255,60), 1));
        p.drawRoundedRect(pr.adjusted(1,1,-1,-1), 4, 4);
        p.setPen(QColor(200, 200, 200));
        p.setFont(QFont("Monospace", 10));
        const QRect labelRect(pr.right() + 8, pr.top(), width() - pr.right() - PAD - 8, PREVIEW_SZ);
        p.drawText(labelRect, Qt::AlignVCenter | Qt::AlignLeft, m_current.name().toUpper());
    }
}

void ColorPickerPopup::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;
    pickColor(event->pos(), /*commit=*/true);
    QWidget::mousePressEvent(event);
}

void ColorPickerPopup::focusOutEvent(QFocusEvent *event) {
    QWidget::focusOutEvent(event);
    hide();
}

void ColorPickerPopup::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) pickColor(event->pos(), /*commit=*/false);
}

void ColorPickerPopup::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) emit colorSelected(m_current);
    QWidget::mouseReleaseEvent(event);
}

void ColorPickerPopup::pickColor(const QPoint &pos, bool commit) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (swatchRect(r, c).adjusted(-2,-2,2,2).contains(pos)) {
                const QColor chosen = m_presets[r * COLS + c];
                m_current = chosen;
                m_hue = chosen.hsvHue() < 0 ? 0 : chosen.hsvHue();
                m_value = chosen.value();
                update();
                if (commit) {
                    emit colorSelected(chosen);
                    hide();
                }
                return;
            }
        }
    }

    const QRect hr = hueRect();
    if (hr.adjusted(0,-4,0,4).contains(pos)) {
        const qreal t = qBound(0.0, static_cast<qreal>(pos.x() - hr.left()) / hr.width(), 1.0);
        m_hue = static_cast<int>(t * 359);
        m_current = QColor::fromHsv(m_hue, 255, m_value);
        update();
        if (commit) emit colorSelected(m_current);
        return;
    }

    const QRect br = brightnessRect();
    if (br.adjusted(0,-4,0,4).contains(pos)) {
        const qreal t = qBound(0.0, static_cast<qreal>(pos.x() - br.left()) / br.width(), 1.0);
        m_value = static_cast<int>(t * 255);
        m_current = QColor::fromHsv(m_hue, 255, m_value);
        update();
        if (commit) emit colorSelected(m_current);
        return;
    }
}