#ifndef SCREENSHOTWRITER_H
#define SCREENSHOTWRITER_H

#include <QObject>
#include <QImage>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ScreenshotLog);

class ScreenshotWriter : public QObject
{
    Q_OBJECT
public:
    explicit ScreenshotWriter(QObject *parent = nullptr);

public slots:
    void setImage(const QImage &img) { _image = img; }

    void takeScreenshot(const QString &filename);


signals:
    void screenshotCreated(const QString &path);
    void screenshotFailed(const QString &error);

private:
    QImage _image;
};

#endif // SCREENSHOTWRITER_H
