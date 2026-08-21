#include "screenshotwriter.h"

#include <QFile>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QLoggingCategory>
#include <QDir>
#include <QFileInfo>


Q_LOGGING_CATEGORY(ScreenshotLog, "stereomaker.screenshot")


ScreenshotWriter::ScreenshotWriter(QObject *parent)
    : QObject{parent}
{}

void ScreenshotWriter::takeScreenshot(const QString &filename)
{
    QString screenshotPath;
    screenshotPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    screenshotPath = screenshotPath + "/StereoMaker";

    QDir screenshotDir(screenshotPath);
    if (!screenshotDir.exists()) {
        if (!screenshotDir.mkpath(screenshotPath)) {
            qCCritical(ScreenshotLog) << "Can't create screenshot dir: " << screenshotPath;
            emit screenshotFailed("Can't create screenshot path:" + screenshotPath);
            return;
        }
    }

    QString fullName = screenshotPath + "/" + filename + ".jpg";

    if (_image.isNull()) {
        qCCritical(ScreenshotLog) << "Screenshot writer called without set image";
        emit screenshotFailed("No image was submitted to screenshot class");
        return;
    }

    if (!_image.save(fullName, "JPG", 95)) {
        qCWarning(ScreenshotLog) << "Can't write screenshot to" << fullName;
        emit screenshotFailed("Can't write screenshot to: " + fullName);
        return;
    }

    qCInfo(ScreenshotLog) << "Screenshot written to" << fullName;
    emit screenshotCreated(fullName);

}
