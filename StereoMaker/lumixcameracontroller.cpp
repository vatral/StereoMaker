#include "lumixcameracontroller.h"

#include <QNetworkDatagram>
#include <QUrlQuery>
#include <QUuid>
#include <QCryptographicHash>
#include <QCoreApplication>


Q_LOGGING_CATEGORY(LumixLog, "stereomaker.controller.lumix")

LumixCameraController::LumixCameraController(QUrl base) : _base(base) {
//    connect(reply, &QIODevice::readyRead, this, &LumixCameraController::reqReadyRead);
//    connect(reply, &QNetworkReply::errorOccurred, this, &LumixCameraController::reqErrorOccurred);
//    connect(reply, &QNetworkReply::sslErrors, this, &LumixCameraController::reqSslErrors);
//    connect(reply, &QNetworkReply::finished, this, &LumixCameraController::reqFinished);

    connect(&_videoSocket, &QIODevice::readyRead, this, &LumixCameraController::videoReadyRead);

}

QNetworkReply* LumixCameraController::makeCameraCommand(const QString &cmd) {
    QUrl url(_base);
    QUrlQuery query;


    query.addQueryItem("mode", "camcmd");
    query.addQueryItem("value", "cmd");


    qCInfo(LumixLog) << "Camera command" << url;
    QNetworkRequest req(url);
    QNetworkReply *reply2 = _manager.get(req);
    return reply2;
}

void LumixCameraController::command(const QString &cmd) {
    QNetworkReply *reply = makeCameraCommand(cmd);


    connect(reply, &QIODevice::readyRead, this, &LumixCameraController::reqReadyRead);
    connect(reply, &QNetworkReply::errorOccurred, this, &LumixCameraController::reqErrorOccurred);
    connect(reply, &QNetworkReply::sslErrors, this, &LumixCameraController::reqSslErrors);
    connect(reply, &QNetworkReply::finished, this, &LumixCameraController::reqFinished);
}

void LumixCameraController::connectToCamera() {

    //
// http://192.168.54.1/cam.cgi?mode=accctrl&type=req_acc&value=4D454930-0100-1000-8001-02FA000430C6&value2=MI%205
// http://192.168.54.1/cam.cgi?mode=setsetting&type=device_name&value=MI%205


    QUrl url(_base);
    QUrlQuery query;


    query.addQueryItem("mode", "accctrl");
    query.addQueryItem("type", "req_acc");
    //query.addQueryItem("value", MAGIC_AUTH_VALUE1);
    //query.addQueryItem("value2", MAGIC_AUTH_VALUE2);
    query.addQueryItem("value", getAppUUID().toString());
    query.addQueryItem("value2", QCoreApplication::applicationName());


    url.setPath("/cam.cgi");
    url.setQuery(query);

    qCInfo(LumixLog) << "First step of authentication, URL" << url;

    QNetworkRequest req(url);
    QNetworkReply *reply = _manager.get(req);


    connect(reply, &QIODevice::readyRead, this, [this,reply]() {
        auto data = reply->readAll();
        qCInfo(LumixLog) << "First step of authentication returned: " << data;


        QUrl url(_base);
        QUrlQuery query;

        url.setPath("/cam.cgi");
        query.addQueryItem("mode", "setsetting");
        query.addQueryItem("type", "device_name");
        query.addQueryItem("value", getAppUUID().toString());
//        query.addQueryItem("value", MAGIC_AUTH_VALUE2);

        url.setQuery(query);


        qCInfo(LumixLog) << "Second step of authentication, URL" << url;

        QNetworkRequest req(url);
        QNetworkReply *reply2 = _manager.get(req);


        connect(reply2, &QIODevice::readyRead, this, [this,reply2]() {
            auto data = reply2->readAll();
            qCInfo(LumixLog) << "Second step of authentication returned: " << data;
            emit connected();



            auto reply3 = makeCameraCommand("");
        });


        connect(reply2, &QNetworkReply::errorOccurred, this, [this,reply2](QNetworkReply::NetworkError code) {
            qCInfo(LumixLog) << "Second step of authentication failed with network error" << code;
            emit connectionFailure();
        });

        connect(reply2, &QNetworkReply::finished, this, [this,reply2]() {
            reply2->deleteLater();
        });

    });

    connect(reply, &QNetworkReply::errorOccurred, this, [this,reply](QNetworkReply::NetworkError code) {
        qCInfo(LumixLog) << "First step of authentication failed with network error" << code;
        emit connectionFailure();
    });

    connect(reply, &QNetworkReply::finished, this, [this,reply]() {
        reply->deleteLater();
    });



}

void LumixCameraController::startStream() {

    _videoSocket.bind(QHostAddress::AnyIPv4);



    QUrl url(_base);
    QUrlQuery query;


   // /cam.cgi?mode=startstream&value=49152


    query.addQueryItem("mode", "startstream");
    query.addQueryItem("value", QString::number(_videoSocket.localPort()));

    //url.setUrl(_base.url());
    url.setPath("/cam.cgi");
    url.setQuery(query);

    qCInfo(LumixLog) << "Starting stream from camera, base URL" << _base << ", URL" << url;

    QNetworkRequest req(url);
    QNetworkReply *reply = _manager.get(req);

    connect(reply, &QIODevice::readyRead, this, &LumixCameraController::reqReadyRead);
    connect(reply, &QNetworkReply::errorOccurred, this, &LumixCameraController::reqErrorOccurred);
    connect(reply, &QNetworkReply::sslErrors, this, &LumixCameraController::reqSslErrors);
    connect(reply, &QNetworkReply::finished, this, &LumixCameraController::reqFinished);



}

void LumixCameraController::stopStream() {
    _videoSocket.close();
}

void LumixCameraController::takePicture() {

}


void LumixCameraController::reqReadyRead() {
    QNetworkReply *req = dynamic_cast<QNetworkReply*>(QObject::sender());


    if (!req) {
        qCCritical(LumixLog) << "Received event from unknown object";
        return;
    }


    qCInfo(LumixLog) << "Received reply to HTTP request: " << req->readAll();
}

void LumixCameraController::videoReadyRead() {
    auto datagram = _videoSocket.receiveDatagram();

    if (!datagram.isValid()) {
        qCCritical(LumixLog) << "Invalid datagram received";
        return;
    }

    qCDebug(LumixLog) << "Received datagram, " << datagram.data().length() << "bytes from" << datagram.senderAddress() << ":" << datagram.senderPort();

    _videoBuffer.append(datagram.data());

    if (_videoBuffer.size() > MAX_BUFFER_LENGTH) {
        qCCritical(LumixLog) << "Maximum buffer length exceeded, starting from scratch";
        _videoBuffer.clear();
        return;
    }

    int start = _videoBuffer.indexOf(START_MARKER);

    if (start < 0 ) {
        // Not enough data yet
        return;
    }

    if (start > 0) {
        qCDebug(LumixLog) << "Removing" << start << "bytes from start";
        _videoBuffer.remove(0, start);
    }

    int end = _videoBuffer.indexOf(END_MARKER);
    if (end < 0) {
        // Not enough data yet
        return;
    }

    QByteArray image_data = _videoBuffer.sliced(0, end+END_MARKER.length());
    _videoBuffer.remove(0, end + END_MARKER.length());


}

void LumixCameraController::reqErrorOccurred(QNetworkReply::NetworkError code) {

}
void LumixCameraController::reqSslErrors(const QList<QSslError> &errors) {

}
void LumixCameraController::reqFinished() {

}


QUuid LumixCameraController::getAppUUID() {
    auto sysid = QSysInfo::machineUniqueId();
    sysid.append(QCoreApplication::applicationName().toUtf8());


    auto hash = QCryptographicHash::hash(sysid, QCryptographicHash::Sha256);
    auto id = QUuid::fromBytes(hash);
    return id;
}
