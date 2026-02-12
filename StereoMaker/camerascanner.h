#ifndef CAMERASCANNER_H
#define CAMERASCANNER_H

#include <QObject>
#include <QList>
#include <QHostAddress>
#include <QLoggingCategory>
#include <QScopedPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

Q_DECLARE_LOGGING_CATEGORY(ScannerLog)

class CameraScanner : public QObject
{
    Q_OBJECT
public:
    explicit CameraScanner(QObject *parent = nullptr);

public slots:
    void scan();
    void makeRequests();

    void reqReadyRead();
    void reqErrorOccurred(QNetworkReply::NetworkError code);
    void reqSslErrors(const QList<QSslError> &errors);
    void reqFinished();

signals:
    void progress(int max, int value);
    void cameraFound(QUrl addr);
    void finished();



private:
    QList<QHostAddress> _queue;
    int _totalRequests = 0;
    int _completed = 0;
    int _found = 0;

    int _inFlight = 0;
    int _maxConcurrency = 64;

    QByteArray _expectedReply;
    QNetworkAccessManager _manager;
};

#endif // CAMERASCANNER_H
