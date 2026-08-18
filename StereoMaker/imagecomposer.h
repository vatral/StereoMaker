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
    enum class StereoMode
    {
        LeftOnly,
        RightOnly,
        SideBySide,
        Anaglyph,
        RowInterlaced,
        ColumnInterlaced,
        Blend50_50
    };

    bool flipSides() const { return _flipSides; }
    void setFlipSides(bool value) { _flipSides = value; }

    StereoMode stereoMode() const { return _mode; }
    void setStereoMode(StereoMode mode) { _mode = mode; }



    explicit ImageComposer(QObject *parent = nullptr);
    void registerPosition(QObject *decoder, int pos);

    void clearPositions() { _positionMapping.clear(); }
public slots:
    void processImage(const QImage &image);

signals:
    void composedImage(const QImage &image);
private:
    QImage composeStereo(const QImage& leftIn, const QImage& rightIn, StereoMode mode);

    QMap<QObject *, int> _positionMapping;
    QList<QImage> _images;
    bool _flipSides = false;
    StereoMode _mode;
};

#endif // IMAGECOMPOSER_H
