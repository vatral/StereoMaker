#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <QObject>

class CameraController : public QObject
{
    Q_OBJECT
public:
    CameraController(const QUrl &base) {}


    /**
     * @brief Issue a command to the camera
     * @param cmd Command to issue
     *
     * Runs asynchronously
     */
    virtual void command(const QString &cmd) = 0;

    QString cameraName() const { return _cameraName; }
protected:
    void setCameraName(const QString &name) { _cameraName = name; }

public slots:
    virtual void connectToCamera() = 0;

    virtual void startStream() = 0;
    virtual void stopStream() = 0;
    virtual void takePicture() = 0;


signals:
    void connected();
    void connectionFailure();
    void imageReceived(const QByteArray &data);

private:
    QString _cameraName{""};
};

#endif // CAMERACONTROLLER_H
