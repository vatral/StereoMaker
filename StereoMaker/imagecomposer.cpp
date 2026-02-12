#include "imagecomposer.h"

Q_LOGGING_CATEGORY(ComposerLog, "stereomaker.composer")

ImageComposer::ImageComposer(QObject *parent)
    : QObject{parent}
{}

void ImageComposer::registerPosition(QObject *decoder, int pos) {
    _positionMapping[decoder] = pos;
}

void ImageComposer::processImage(const QImage &image) {
    QObject *sender = QObject::sender();

    if (!_positionMapping.contains(sender)) {
        qCCritical(ComposerLog) << "Got signal from unregistered object" << sender;
        return;
    }


    int pos = _positionMapping[sender];




}
