#include "camerascanner.h"

#include <QNetworkRequest>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QDebug>
#include <QSslError>
#include <QNetworkReply>


Q_LOGGING_CATEGORY(ScannerLog, "stereomaker.scanner")


// Build a netmask from prefix length (0..32)
static inline quint32 maskFromPrefix(int prefix)
{
    if (prefix <= 0)  return 0u;
    if (prefix >= 32) return 0xFFFFFFFFu;
    return 0xFFFFFFFFu << (32 - prefix);
}


// Enumerate all usable IPv4 addresses in a subnet (skips network & broadcast for /0–/30)
static QList<QHostAddress> enumerateSubnet(const QHostAddress& networkAddr, int prefix)
{
    QList<QHostAddress> out;

    // Only IPv4 enumeration (IPv6 subnets are astronomically large to iterate)
    if (networkAddr.protocol() != QAbstractSocket::IPv4Protocol || prefix < 0 || prefix > 32) {
        return out;
    }

    const quint32 netMask = maskFromPrefix(prefix);
    const quint32 base    = networkAddr.toIPv4Address() & netMask;
    const quint32 bcast   = base | (~netMask);

    // Special cases: /31 and /32 (RFC 3021)
    if (prefix == 32) {
        out.append(QHostAddress(base));
        return out;
    }
    if (prefix == 31) {
        out.append(QHostAddress(base));
        out.append(QHostAddress(bcast));
        return out;
    }

    // Typical case: /0.. /30 — iterate inside (skip network and broadcast)
    for (quint32 ip = base + 1; ip < bcast; ++ip)
        out.append(QHostAddress(ip));

    return out;
}



CameraScanner::CameraScanner(QObject *parent)
    : QObject{parent} {

    _manager.setTransferTimeout(2000);

    _expectedReply =
    "<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\n"
    "<BODY BGCOLOR=\"#cc9999\"><H2>403 Forbidden</H2>\n"
    "<HR>\n"
    "You do not have permission to get requested URL from this server.\n"
    "</BODY></HTML>\n";

}

void CameraScanner::scan() {
    _queue.clear();
    _inFlight = 0;
    _completed = 0;
    _totalRequests = 0;



    for(const auto& interface : QNetworkInterface::allInterfaces()) {
        qInfo() << "Checking interface" << interface.name();

//        if (interface.IsLoopBack) {
//            qCWarning(ScannerLog) << "Local interface, skipping:" << interface.IsLoopBack;
//           // continue;
//        }

        if (!interface.IsRunning) {
            qCWarning(ScannerLog) << "Interface isn't running, skipping";
            continue;
        }

        if (!interface.IsUp) {
            qCWarning(ScannerLog) << "Interface isn't up, skipping";
            continue;
        }

        for (const auto &entry :  interface.addressEntries()) {
            qCInfo(ScannerLog) << "Checking entry" << entry;

            if (entry.ip().protocol() != QAbstractSocket::IPv4Protocol) {
                qCWarning(ScannerLog) << "Entry is not IPv4, skipping";
                continue;
            }

            if (entry.ip().isLoopback()) {
                qCWarning(ScannerLog) << "Entry is loopback, skipping";
                continue;
            }

            if (entry.prefixLength() < 16) {
                qCWarning(ScannerLog) << "Prefix length is less than 16, too many addresses to scan, skipping.";
                continue;
            }

            qCInfo(ScannerLog) << "Adding entry:" << entry;

            auto addresses = enumerateSubnet(entry.ip(), entry.prefixLength());
            _queue.append(addresses);


        }

    }

    qCInfo(ScannerLog) << "Queued up" << _queue.length() << "addresses to scan";
    _totalRequests = _queue.length();

    emit progress(_totalRequests, 0);


    makeRequests();
}


void CameraScanner::makeRequests() {
    while(_inFlight < _maxConcurrency && !_queue.isEmpty()) {
        auto addr = _queue.takeFirst();


        QUrl url = QUrl(QString("http://%1").arg(addr.toString()));
        QNetworkRequest req(url);


        qCInfo(ScannerLog) << "Requesting" << url;
        QNetworkReply *reply = _manager.get(req);


        connect(reply, &QIODevice::readyRead, this, &CameraScanner::reqReadyRead);
        connect(reply, &QNetworkReply::errorOccurred, this, &CameraScanner::reqErrorOccurred);
        connect(reply, &QNetworkReply::sslErrors, this, &CameraScanner::reqSslErrors);
        connect(reply, &QNetworkReply::finished, this, &CameraScanner::reqFinished);


        _inFlight++;
    }


}



void CameraScanner::reqReadyRead() {
    QNetworkReply *req = dynamic_cast<QNetworkReply*>(QObject::sender());


    qCInfo(ScannerLog) << "ReadyRead on" << req->url();
    auto data = req->readAll();

    qCInfo(ScannerLog) << "received" << data;

    if ( data == _expectedReply ) {
        _found++;
        qCInfo(ScannerLog) << "Found camera!";
        emit cameraFound(req->url());
    }



    //auto data =

}
void CameraScanner::reqErrorOccurred(QNetworkReply::NetworkError code) {
    QNetworkReply *req = dynamic_cast<QNetworkReply*>(QObject::sender());


    qCInfo(ScannerLog) << "Error" << code << "on" << req->url();

}

void CameraScanner::reqSslErrors(const QList<QSslError> &errors) {

    qCInfo(ScannerLog) << "SSL errors" << errors;

}
void CameraScanner::reqFinished() {
    QNetworkReply *req = dynamic_cast<QNetworkReply*>(QObject::sender());


    qCInfo(ScannerLog) << "Finished" << req->url() << "; found" << _found << "cameras";

    _inFlight--;
    _completed++;
    emit progress(_totalRequests, _completed);

    makeRequests();

    if (_queue.isEmpty() && _inFlight == 0) {
        emit finished();
    }

    req->deleteLater();
}
