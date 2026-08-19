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

#include "cameracontroller.h"

Q_DECLARE_LOGGING_CATEGORY(LumixLog)


class LumixCameraController : public CameraController
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
    virtual void command(const QString &cmd) override;

public slots:
    virtual void connectToCamera() override;

    virtual void startStream() override;
    virtual void stopStream() override;
    virtual void takePicture() override;

    void videoReadyRead();

    void reqReadyRead();
    void reqErrorOccurred(QNetworkReply::NetworkError code);
    void reqSslErrors(const QList<QSslError> &errors);
    void reqFinished();


signals:
    void connected();
    void connectionFailure();
    void imageReceived(const QByteArray &data);

private slots:
    void stateTimerFired();


private:
    /*
       Sample of state output:

       <camrply>
        <result>ok</result>
        <state>
        <batt>3/3</batt>
        <cammode>rec</cammode>
        <remaincapacity>2817</remaincapacity>
        <sdcardstatus>write_enable</sdcardstatus>
        <sd_memory>set</sd_memory>
        <video_remaincapacity>300</video_remaincapacity>
        <rec>off</rec>
        <burst_interval_status>off</burst_interval_status>
        <sd_access>off</sd_access>
        <rem_disp_typ>num</rem_disp_typ>
        <progress_time>0</progress_time>
        <operate>enable/enable</operate>
        <stop_motion_num>0</stop_motion_num>
        <stop_motion>off</stop_motion>
        <temperature>low</temperature>
        <lens>normal</lens>
        <add_location_data>off</add_location_data>
        <interval_status>off</interval_status>
        <sdi_state>none</sdi_state>
        <sd2_cardstatus>write_enable</sd2_cardstatus>
        <sd2_memory>unset</sd2_memory>
        <sd2_access>off</sd2_access>
        <current_sd>sd1</current_sd>
        <backupmode>off</backupmode>
        <batt_grip>-1/0</batt_grip>
        <warn_disp>no_disp</warn_disp>
        <cinelike>off</cinelike>
        <version>D2.91</version>
        </state>
        </camrply>
    */

    /**
     * @brief The StateInfo class
     */
    struct StateInfo {

    };

    /**
     * @brief Do through the motions of making a camera command request, for internal reuse
     * @param cmd Camera command to issue
     * @return Reply object
     */
    QNetworkReply* makeCameraCommand(const QString &cmd, const QString &arg = QString());


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

    const int STATE_TIMER_INTERVAL = 1000;

    QTimer _stateTimer;



};

#endif // LUMIXCAMERACONTROLLER_H
