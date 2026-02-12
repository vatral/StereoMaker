#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QLoggingCategory>
#include <QSharedPointer>
#include <QScopedPointer>


#include "camerascanner.h"
#include "imagecomposer.h"
#include "imagedecoder.h"
#include "lumixcameracontroller.h"


Q_DECLARE_LOGGING_CATEGORY(MainLog)


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void scanClicked();
    void scanProgress(int max, int value);
    void scanFinished();
    void scanFoundCamera(QUrl base);



private:
    Ui::MainWindow *ui;
    CameraScanner _camScanner;
    QList<QSharedPointer<LumixCameraController>> _cameras;
    QList<QSharedPointer<ImageDecoder>> _decoders;
    ImageComposer _composer;


};
#endif // MAINWINDOW_H
