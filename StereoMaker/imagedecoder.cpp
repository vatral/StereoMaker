#include "imagedecoder.h"
#include <QImage>

Q_LOGGING_CATEGORY(DecoderLog, "stereomaker.decoder")

ImageDecoder::ImageDecoder(QObject *parent)
    : QObject{parent}
{}

void ImageDecoder::processImageData(const QByteArray &data) {
    QImage img;

    if (img.loadFromData(data)) {
        emit decodedImage(img);
    } else {
        emit decodingFailed();
    }
}
