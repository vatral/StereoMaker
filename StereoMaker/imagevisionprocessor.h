#ifndef IMAGEVISIONPROCESSOR_H
#define IMAGEVISIONPROCESSOR_H

#include <QObject>
#include <QImage>


class ImageVisionProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageVisionProcessor(QObject *parent = nullptr);

public slots:

    void processImage(const QImage &img);

signals:

    void processedImage(const QImage &img);
};

#endif // IMAGEVISIONPROCESSOR_H
