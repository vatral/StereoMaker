#include "imagecomposer.h"

#include <QImage>
#include <QPainter>
#include <QtGlobal>


Q_LOGGING_CATEGORY(ComposerLog, "stereomaker.composer")


static QImage toArgb32(const QImage& img) {
    return (img.format() == QImage::Format_ARGB32 || img.format() == QImage::Format_RGB32)
    ? img
    : img.convertToFormat(QImage::Format_ARGB32);
}

ImageComposer::ImageComposer(QObject *parent)
    : QObject{parent}
{}

void ImageComposer::registerPosition(QObject *decoder, int pos) {
    _positionMapping[decoder] = pos;
    _images.resize(_positionMapping.count());
}

void ImageComposer::processImage(const QImage &image) {
    QObject *sender = QObject::sender();

    if (!_positionMapping.contains(sender)) {
        qCCritical(ComposerLog) << "Got signal from unregistered object" << sender;
        return;
    }


    if (_images.length() == 0) {
        qCCritical(ComposerLog) << "No registered sources!";
        return;
    }

    int pos = _positionMapping[sender];

    _images[pos] = image;

    if (_images.length() == 1) {
        // Only one source, output as-is
        emit composedImage(_images[pos]);
    } else if (_images.length() == 2) {
        QImage out;

        if (_flipSides) {
            out = composeStereo(_images[1], _images[0], _mode);
        } else {
            out = composeStereo(_images[0], _images[1], _mode);
        }

        emit composedImage(out);
    } else {
        qCWarning(ComposerLog) << "Don't know how to handle more than two cameras yet";
    }
}




QImage ImageComposer::composeStereo(const QImage& leftIn, const QImage& rightIn, StereoMode mode) {
    if (leftIn.isNull() && rightIn.isNull())
        return {};

    if (leftIn.isNull())
        return toArgb32(rightIn);

    if (rightIn.isNull())
        return toArgb32(leftIn);

    QImage left  = toArgb32(leftIn);
    QImage right = toArgb32(rightIn);

    const QSize targetSize = left.size();
    if (right.size() != targetSize)
        right = right.scaled(targetSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    switch (mode) {
        case StereoMode::LeftOnly:
            return left;

        case StereoMode::RightOnly:
            return right;

        case StereoMode::SideBySide: {
            QImage out(targetSize.width() * 2, targetSize.height(), QImage::Format_ARGB32);
            out.fill(Qt::black);

            QPainter p(&out);
            p.drawImage(0, 0, left);
            p.drawImage(targetSize.width(), 0, right);
            return out;
        }

        case StereoMode::Anaglyph: {
            QImage out(targetSize, QImage::Format_ARGB32);

            for (int y = 0; y < targetSize.height(); ++y) {
                const QRgb* lRow = reinterpret_cast<const QRgb*>(left.constScanLine(y));
                const QRgb* rRow = reinterpret_cast<const QRgb*>(right.constScanLine(y));
                QRgb* oRow       = reinterpret_cast<QRgb*>(out.scanLine(y));

                for (int x = 0; x < targetSize.width(); ++x) {
                    const int r = qRed(lRow[x]);    // red from left
                    const int g = qGreen(rRow[x]);  // green from right
                    const int b = qBlue(rRow[x]);   // blue from right
                    oRow[x] = qRgba(r, g, b, 255);
                }
            }
            return out;
        }

        case StereoMode::RowInterlaced: {
            // TODO: Scale to viewport
            QImage out(targetSize, QImage::Format_ARGB32);

            for (int y = 0; y < targetSize.height(); ++y) {
                const bool useLeft = (y % 2 == 0);
                const uchar* src   = useLeft ? left.constScanLine(y) : right.constScanLine(y);
                std::memcpy(out.scanLine(y), src, static_cast<size_t>(out.bytesPerLine()));
            }
            return out;
        }

        case StereoMode::ColumnInterlaced: {
            // TODO: Scale to viewport
            QImage out(targetSize, QImage::Format_ARGB32);

            for (int y = 0; y < targetSize.height(); ++y) {
                const QRgb* lRow = reinterpret_cast<const QRgb*>(left.constScanLine(y));
                const QRgb* rRow = reinterpret_cast<const QRgb*>(right.constScanLine(y));
                QRgb* oRow       = reinterpret_cast<QRgb*>(out.scanLine(y));

                for (int x = 0; x < targetSize.width(); ++x) {
                    oRow[x] = (x % 2 == 0) ? lRow[x] : rRow[x];
                }
            }
            return out;
        }

        case StereoMode::Blend50_50: {
            QImage out(targetSize, QImage::Format_ARGB32);

            for (int y = 0; y < targetSize.height(); ++y) {
                const QRgb* lRow = reinterpret_cast<const QRgb*>(left.constScanLine(y));
                const QRgb* rRow = reinterpret_cast<const QRgb*>(right.constScanLine(y));
                QRgb* oRow       = reinterpret_cast<QRgb*>(out.scanLine(y));

                for (int x = 0; x < targetSize.width(); ++x) {
                    const int r = (qRed(lRow[x])   + qRed(rRow[x]))   / 2;
                    const int g = (qGreen(lRow[x]) + qGreen(rRow[x])) / 2;
                    const int b = (qBlue(lRow[x])  + qBlue(rRow[x]))  / 2;
                    const int a = (qAlpha(lRow[x]) + qAlpha(rRow[x])) / 2;
                    oRow[x] = qRgba(r, g, b, a);
                }
            }
            return out;
        }
    }

    return left; // fallback
}

