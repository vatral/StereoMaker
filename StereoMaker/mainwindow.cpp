#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lumixcameracontroller.h"
#include "findcamerasdialog.h"
#include "fakecameracontroller.h"

#include <QButtonGroup>

Q_LOGGING_CATEGORY(MainLog, "stereomaker.mainwindow")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QButtonGroup *imageButtonGroup = new QButtonGroup(this);
    imageButtonGroup->setExclusive(true);

    imageButtonGroup->addButton(ui->leftImageButton);
    imageButtonGroup->addButton(ui->rightImageButton);
    imageButtonGroup->addButton(ui->sideBySideButton);
    imageButtonGroup->addButton(ui->anaglyphButton);
    imageButtonGroup->addButton(ui->transparencyButton);

    ui->anaglyphButton->setChecked(true);

    connect(&_camScanner, &CameraScanner::progress, this, &MainWindow::scanProgress);
    connect(&_camScanner, &CameraScanner::finished, this, &MainWindow::scanFinished);
    connect(&_camScanner, &CameraScanner::cameraFound, this, &MainWindow::scanFoundCamera);

    connect(&_composer, &ImageComposer::composedImage, this, &MainWindow::showComposedImage);
    qCInfo(MainLog) << "Started";
    qCDebug(MainLog) << "Debug test";

    _composer.setStereoMode(ImageComposer::StereoMode::Anaglyph);

    connect(ui->leftImageButton, &QPushButton::clicked, [this]() { _composer.setStereoMode(ImageComposer::StereoMode::LeftOnly);});
    connect(ui->rightImageButton, &QPushButton::clicked, [this]() { _composer.setStereoMode(ImageComposer::StereoMode::RightOnly);});
    connect(ui->sideBySideButton, &QPushButton::clicked, [this]() { _composer.setStereoMode(ImageComposer::StereoMode::SideBySide);});
    connect(ui->anaglyphButton, &QPushButton::clicked, [this]() { _composer.setStereoMode(ImageComposer::StereoMode::Anaglyph);});
    connect(ui->transparencyButton, &QPushButton::clicked, [this]() { _composer.setStereoMode(ImageComposer::StereoMode::Blend50_50);});

    setupFakeCameras();
}

MainWindow::~MainWindow()
{
    delete ui;



}

void MainWindow::scanClicked() {
    qCInfo(MainLog) << "Scan clicked";

    ui->scanButton->setEnabled(false);
    _cameras.clear();
    _decoders.clear();
    _composer.clearPositions();

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

void MainWindow::showScanWindow() {
    qCInfo(MainLog) << "Showing scan window";

    FindCamerasDialog *findCams = new FindCamerasDialog(this);
    findCams->setWindowModality(Qt::WindowModal);
    findCams->show();

}

void MainWindow::setupFakeCameras()
{
    qCInfo(MainLog) << "Setting up fake cameras";

    _cameras.clear();
    _decoders.clear();
    _composer.clearPositions();

    FakeCameraController *leftCamera = new FakeCameraController(QUrl());
    FakeCameraController *rightCamera = new FakeCameraController(QUrl());
    ImageDecoder *leftDecoder = new ImageDecoder();
    ImageDecoder *rightDecoder = new ImageDecoder();


    leftCamera->loadImage(QCoreApplication::applicationDirPath() +  "/test_images/left.jpg");
    rightCamera->loadImage(QCoreApplication::applicationDirPath() + "/test_images/right.jpg");

    connect(leftCamera, &CameraController::connected, this, [this,leftCamera]() { leftCamera->startStream(); } );
    connect(rightCamera, &CameraController::connected, this, [this,rightCamera]() { rightCamera->startStream(); } );


    connect(leftCamera, &CameraController::imageReceived, leftDecoder, &ImageDecoder::processImageData);
    connect(rightCamera, &CameraController::imageReceived, rightDecoder, &ImageDecoder::processImageData);

    _composer.registerPosition(leftDecoder, 0);
    _composer.registerPosition(rightDecoder, 1);

    connect(leftDecoder, &ImageDecoder::decodedImage, &_composer, &ImageComposer::processImage);
    connect(rightDecoder, &ImageDecoder::decodedImage, &_composer, &ImageComposer::processImage);

    _cameras.append(QSharedPointer<CameraController>(leftCamera));
    _cameras.append(QSharedPointer<CameraController>(rightCamera));

    _decoders.append(QSharedPointer<ImageDecoder>(leftDecoder));
    _decoders.append(QSharedPointer<ImageDecoder>(rightDecoder));


    leftCamera->connectToCamera();
    rightCamera->connectToCamera();

}

void MainWindow::showComposedImage(const QImage &img)
{
    QPixmap pix = QPixmap::fromImage(img);
    ui->imageLabel->setPixmap(pix);
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
