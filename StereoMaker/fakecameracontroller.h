#ifndef FAKECAMERACONTROLLER_H
#define FAKECAMERACONTROLLER_H

#include <QObject>
#include "cameracontroller.h"
#include <QTimer>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(FakeCamLog);



class FakeCameraController : public CameraController
{
    Q_OBJECT
public:
    FakeCameraController(const QUrl &base);


    /**
     * @brief Issue a command to the camera
     * @param cmd Command to issue
     *
     * Runs asynchronously
     */
    virtual void command(const QString &cmd) override;

    int offset() const { return _offset; }

    void setOffset(int offset) { _offset=offset; }

    int width() const { return _width; }

    void setWidth(int width) { _width = width; }

    int height() const { return _height; }

    void setHeight(int height) { _height = height; }

    void setInterval(std::chrono::milliseconds ms) { _streamTimer.setInterval(ms);}

    bool loadImage(const QString &img);

public slots:
    virtual void connectToCamera() override;

    virtual void startStream() override;
    virtual void stopStream() override;
    virtual void takePicture() override;
private slots:

    void timerElapsed();

private:
    bool _connected{false};
    bool _streaming{false};
    int _offset{0};
    int _width{1364};
    int _height{1024};
    const std::chrono::milliseconds DEFAULT_INTERVAL{100};

    QByteArray _imageData;
    QTimer _streamTimer;
};

#endif // FAKECAMERACONTROLLER_H
