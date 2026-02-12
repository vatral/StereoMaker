#ifndef IMAGECOMPOSER_H
#define IMAGECOMPOSER_H

#include <QObject>
#include <QMap>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ComposerLog)

class ImageComposer : public QObject
{
    Q_OBJECT
public:
    explicit ImageComposer(QObject *parent = nullptr);
    void registerPosition(QObject *decoder, int pos);

public slots:
    void processImage(const QImage &image);

signals:
    void composedImage(const QImage &image);
private:
    QMap<QObject *, int> _positionMapping;
};

#endif // IMAGECOMPOSER_H
