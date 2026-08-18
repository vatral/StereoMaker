#ifndef IMAGEVISIONPROCESSOR_H
#define IMAGEVISIONPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(VisionProcessor)


class ImageVisionProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageVisionProcessor(QObject *parent = nullptr);

    void setEnabled(bool enabled) { _enabled = enabled; }

    bool enabled() const { return _enabled; }

public slots:

    void processImage(const QImage &img);

signals:

    void processedImage(const QImage &img);
private:
    bool _enabled{false};

};

#endif // IMAGEVISIONPROCESSOR_H
