#include "fakecameracontroller.h"
#include <QFile>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(FakeCamLog, "stereomaker.controller.fake");



FakeCameraController::FakeCameraController(const QUrl &url) : CameraController(url) {
    connect(&_streamTimer, &QTimer::timeout, this, &FakeCameraController::timerElapsed);
    qCInfo(FakeCamLog) << "Created";

    _streamTimer.setInterval(DEFAULT_INTERVAL);
    _streamTimer.setSingleShot(false);

}

void FakeCameraController::command(const QString &cmd) {
    qCInfo(FakeCamLog) << "Fake camera command:" << cmd;

}

void FakeCameraController::connectToCamera() {
    qCInfo(FakeCamLog) << "Fake camera connected";
    emit connected();
};

void FakeCameraController::startStream() {
    qCInfo(FakeCamLog) << "Starting stream";
    _streamTimer.start();
}
void FakeCameraController::stopStream() {
    qCInfo(FakeCamLog) << "Stopping stream";

    _streamTimer.stop();
}

void FakeCameraController::takePicture() {
    qCInfo(FakeCamLog) << "Taking picture";

}

void FakeCameraController::timerElapsed()
{
    emit imageReceived(_imageData);
}

bool FakeCameraController::loadImage(const QString &img) {
    QFile image(img);

    if (!image.open(QIODevice::ReadOnly)) {
        qCCritical(FakeCamLog) << "Loading image" << img << "failed";
        return false;
    }

    _imageData = image.readAll();

    qCInfo(FakeCamLog) << "Loaded image" << img;

    return true;
}