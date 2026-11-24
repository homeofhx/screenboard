#include "ImageExporter.h"
#include <QImage>
#include <QSaveFile>
#include <QCoreApplication>

QString ImageExporter::defaultFileName(const QDateTime &dateTime) {
    return QStringLiteral("ScreenBoardContent_%1.png").arg(dateTime.toString(QStringLiteral("MMddyyyy-HHmmss")));
}

QString ImageExporter::pngFileName(const QString &fileName) {
    return fileName.endsWith(QStringLiteral(".png"), Qt::CaseInsensitive) ? fileName : fileName + QStringLiteral(".png");
}

bool ImageExporter::savePng(const QImage &image, const QString &fileName, QString *error) {
    if (error) error->clear();

    auto fail = [error](const QString &message) {
        if (error) *error = message;
        return false;
    };
    if (image.isNull()) return fail(QCoreApplication::translate("ImageExporter", "The whiteboard image is empty."));

    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return fail(file.errorString());
    if (!image.save(&file, "PNG")) {
        file.cancelWriting();
        return fail(QCoreApplication::translate("ImageExporter", "Unable to encode the whiteboard as PNG."));
    }
    if (!file.commit()) return fail(file.errorString());

    return true;
}