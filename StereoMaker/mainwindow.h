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
#include "imagevisionprocessor.h"

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
    void showScanWindow();

    void setupFakeCameras();


    void showComposedImage(const QImage &img);

    void enableImageProcessors();

    void takePictureClicked();


private:
    Ui::MainWindow *ui;
    CameraScanner _camScanner;
    QList<QSharedPointer<CameraController>> _cameras;
    QList<QSharedPointer<ImageDecoder>> _decoders;
    QList<QSharedPointer<ImageVisionProcessor>> _visions;

    ImageComposer _composer;


};
#endif // MAINWINDOW_H
