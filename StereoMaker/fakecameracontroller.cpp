#include "fakecameracontroller.h"

FakeCameraController::FakeCameraController(const QUrl &url) : CameraController(url) {

}

void FakeCameraController::command(const QString &cmd) {

}

void FakeCameraController::connectToCamera() {
    emit connected();
};

void FakeCameraController::startStream() {
    _streamTimer.start();
}
void FakeCameraController::stopStream() {

}
void FakeCameraController::takePicture() {

}