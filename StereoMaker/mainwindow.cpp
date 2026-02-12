#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lumixcameracontroller.h"

Q_LOGGING_CATEGORY(MainLog, "stereomaker.mainwindow")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&_camScanner, &CameraScanner::progress, this, &MainWindow::scanProgress);
    connect(&_camScanner, &CameraScanner::finished, this, &MainWindow::scanFinished);
    connect(&_camScanner, &CameraScanner::cameraFound, this, &MainWindow::scanFoundCamera);

    qCInfo(MainLog) << "Started";
    qCDebug(MainLog) << "Debug test";
}

MainWindow::~MainWindow()
{
    delete ui;



}

void MainWindow::scanClicked() {
    qCInfo(MainLog) << "Scan clicked";

    ui->scanButton->setEnabled(false);
    _camScanner.scan();

}


void MainWindow::scanProgress(int max, int value) {
    ui->scanProgress->setMaximum(max);
    ui->scanProgress->setValue(value);

    qCInfo(MainLog) << "Progress:" << value << "of" << max;
}

void MainWindow::scanFinished() {
    ui->scanButton->setEnabled(true);
    qCInfo(MainLog) << "Scan finished";
}

void MainWindow::scanFoundCamera(QUrl url) {
    qCInfo(MainLog) << "Found camera at" << url;

    LumixCameraController *cam = new LumixCameraController(url);
    ImageDecoder *decoder = new ImageDecoder();

    // HACK, fix later
    _composer.registerPosition(decoder, _cameras.length());

    connect(cam, &LumixCameraController::imageReceived, decoder, &ImageDecoder::processImageData);
    connect(cam, &LumixCameraController::connected, this, [this,cam]() { cam->startStream(); } );

    connect(decoder, &ImageDecoder::decodedImage, &_composer, &ImageComposer::processImage);


    _cameras.append(QSharedPointer<LumixCameraController>(cam));
    _decoders.append(QSharedPointer<ImageDecoder>(decoder));

    cam->connectToCamera();


}
