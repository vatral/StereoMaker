#include "imagevisionprocessor.h"

#include <QDebug>
#include <QStringList>

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/imgproc.hpp>

Q_LOGGING_CATEGORY(VisionProcessor, "stereomaker.vision")


ImageVisionProcessor::ImageVisionProcessor(QObject *parent)
    : QObject{parent}
{}

void ImageVisionProcessor::processImage(const QImage &img)
{
    if (img.isNull()) {
        qCWarning(VisionProcessor) << "ChArUco: input image is null";
        emit processedImage(img);
        return;
    }

    if (!_enabled) {
        emit processedImage(img);
        return;
    }

    const QImage rgb = img.convertToFormat(QImage::Format_RGB888);
    cv::Mat rgbMat(rgb.height(), rgb.width(), CV_8UC3,
                   const_cast<uchar *>(rgb.constBits()),
                   static_cast<size_t>(rgb.bytesPerLine()));

    cv::Mat gray;
    cv::cvtColor(rgbMat, gray, cv::COLOR_RGB2GRAY);

    cv::aruco::Dictionary dictionary =
        cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::aruco::CharucoBoard board(cv::Size(5, 7), 0.04f, 0.02f, dictionary);
    cv::aruco::CharucoDetector charucoDetector(board);

    std::vector<std::vector<cv::Point2f>> markerCorners;
    std::vector<int> markerIds;
    cv::aruco::ArucoDetector arucoDetector(dictionary);
    arucoDetector.detectMarkers(gray, markerCorners, markerIds);

    if (markerIds.empty()) {
        //qInfo() << "ChArUco: no ArUco markers detected";
        return;
    }

    cv::Mat charucoCorners;
    cv::Mat charucoIds;
    charucoDetector.detectBoard(gray, charucoCorners, charucoIds,
                                 markerCorners, markerIds);
    const int interpolated = charucoIds.rows;

    QStringList markerIdStrings;
    markerIdStrings.reserve(static_cast<int>(markerIds.size()));
    for (int id : markerIds) {
        markerIdStrings << QString::number(id);
    }

    QStringList charucoIdStrings;
    charucoIdStrings.reserve(charucoIds.rows);
    for (int i = 0; i < charucoIds.rows; ++i) {
        charucoIdStrings << QString::number(charucoIds.at<int>(i));
    }

    qCInfo(VisionProcessor) << "ChArUco: markers=" << markerIds.size()
            << "markerIds=[" + markerIdStrings.join(",") + "]"
            << "charucoCorners=" << interpolated
            << "charucoIds=[" + charucoIdStrings.join(",") + "]";

    emit processedImage(img);
}
