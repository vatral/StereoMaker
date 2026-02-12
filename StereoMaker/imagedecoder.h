#ifndef IMAGEDECODER_H
#define IMAGEDECODER_H

#include <QObject>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(DecoderLog)

class ImageDecoder : public QObject
{
    Q_OBJECT
public:
    explicit ImageDecoder(QObject *parent = nullptr);

public slots:
    void processImageData(const QByteArray &data);

signals:
    void decodedImage(const QImage &image);
    void decodingFailed();
};

#endif // IMAGEDECODER_H
