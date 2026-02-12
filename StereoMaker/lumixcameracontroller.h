#ifndef LUMIXCAMERACONTROLLER_H
#define LUMIXCAMERACONTROLLER_H

#include <QObject>
#include <QHostAddress>
#include <QUrl>
#include <QNetworkReply>
#include <QUdpSocket>
#include <QLoggingCategory>
#include <QByteArray>
#include <QTimer>
#include <QUuid>

Q_DECLARE_LOGGING_CATEGORY(LumixLog)


class LumixCameraController : public QObject
{
    Q_OBJECT
public:
    LumixCameraController(QUrl base);

    /**
     * @brief Issue a command to the camera
     * @param cmd Command to issue
     *
     * Runs asynchronously
     */
    void command(const QString &cmd);

public slots:
    void connectToCamera();

    void startStream();
    void stopStream();
    void takePicture();

    void videoReadyRead();

    void reqReadyRead();
    void reqErrorOccurred(QNetworkReply::NetworkError code);
    void reqSslErrors(const QList<QSslError> &errors);
    void reqFinished();


signals:
    void connected();
    void connectionFailure();
    void imageReceived(const QByteArray &data);



private:

    /**
     * @brief Do through the motions of making a camera command request, for internal reuse
     * @param cmd Camera command to issue
     * @return Reply object
     */
    QNetworkReply* makeCameraCommand(const QString &cmd);


    /**
     * @brief Return an unique ID for the application and computer
     *
     * The Lumix protocol wants us to provide an unique ID. We make one
     * from the computer's ID and our own app name.
     *
     * @return UUID
     */
    QUuid getAppUUID();


    QUrl _base;
    QNetworkAccessManager _manager;

    QUdpSocket _videoSocket;
    QByteArray _videoBuffer;

    QTimer _videoTimeoutTimer;
    QTimer _keepaliveTimer;

    const QByteArray START_MARKER = QByteArray::fromHex("ffd8");
    const QByteArray END_MARKER = QByteArray::fromHex("ffd9");
    const int MAX_BUFFER_LENGTH = 1024*1024*32;
    const int VIDEO_TIMEOUT_MS = 3000;
    const int KEEPALIVE_MS = 10000;



    // Authentication for the Lumix protocol
    const QString MAGIC_AUTH_VALUE1{"4D454930-0100-1000-8001-02FA000430C6"};
    const QString MAGIC_AUTH_VALUE2{"MI 5"};




};

#endif // LUMIXCAMERACONTROLLER_H
