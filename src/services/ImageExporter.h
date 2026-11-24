#pragma once
#include <QDateTime>
#include <QString>

class QImage;

namespace ImageExporter {
QString defaultFileName(const QDateTime &dateTime = QDateTime::currentDateTime());
QString pngFileName(const QString &fileName);
bool savePng(const QImage &image, const QString &fileName, QString *error = nullptr);
}