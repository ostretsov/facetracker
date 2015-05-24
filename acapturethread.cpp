#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <QtGui/QImage>

#include "acapturethread.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ACaptureThread::ACaptureThread(QObject *parent)
    : QThread(parent), _dev_idx(0), _det_min(20), _det_max(50)
    , _hidden_capture(true), _hidden_detect(true) {}


// ========================================================================== //
// Get detector minimum size.
// ========================================================================== //
int ACaptureThread::detectorMinSize() {
    QMutexLocker locker(&_mutex); return _det_min;
}


// ========================================================================== //
// Get detector maximum size.
// ========================================================================== //
int ACaptureThread::detectorMaxSize() {
    QMutexLocker locker(&_mutex); return _det_max;
}


// ========================================================================== //
// Get capture is hidden.
// ========================================================================== //
bool ACaptureThread::isHiddenCapture() {
    QMutexLocker locker(&_mutex); return _hidden_capture;
}


// ========================================================================== //
// Get detection is hidden.
// ========================================================================== //
bool ACaptureThread::isHiddenDetect() {
    QMutexLocker locker(&_mutex); return _hidden_detect;
}


// ========================================================================== //
// Set detector minimum size.
// ========================================================================== //
void ACaptureThread::setDetectorMinSize(int min) {
    QMutexLocker locker(&_mutex); _det_min = min;
}


// ========================================================================== //
// Set detector maximum size.
// ========================================================================== //
void ACaptureThread::setDetectorMaxSize(int max) {
    QMutexLocker locker(&_mutex); _det_max = max;
}


// ========================================================================== //
// Set capture hidden.
// ========================================================================== //
void ACaptureThread::setCaptureHidden(bool hidden) {
    QMutexLocker locker(&_mutex); _hidden_capture = hidden;
}


// ========================================================================== //
// Set detection hidden.
// ========================================================================== //
void ACaptureThread::setDetectHidden(bool hidden) {
    QMutexLocker locker(&_mutex); _hidden_detect = hidden;
}


// ========================================================================== //
// Run.
// ========================================================================== //
void ACaptureThread::run() {
    _mutex.lock();
    const int dev_idx = _dev_idx;
    _mutex.unlock();

    cv::VideoCapture capture(dev_idx);
    if(!capture.isOpened()) {
        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "app").warning()
            << qPrintable(ACaptureThread::tr("Capturing with device %1 failed!")
                .arg(dev_idx));

        return;
    }

    const QString src_fname(":/classifiers/haar_frontalface_alt_tree.xml");

    QTemporaryFile *dst_file = QTemporaryFile::createNativeFile(src_fname);
    if(!dst_file) {
        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "app").warning()
            << qPrintable(ACaptureThread::tr("Can not create temporary file!"));

        return;
    }

    const QString dst_fname = QDir::toNativeSeparators(dst_file->fileName());

    cv::CascadeClassifier classifier;
    if(!classifier.load(dst_fname.toStdString())) {
        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "app").warning()
            << qPrintable(ACaptureThread::tr("Load classifier failed!"));

        return;
    }

    delete dst_file;

    cv::Mat acc_mat;

    while(!isInterruptionRequested()) {
        cv::Mat src_mat;
        capture >> src_mat;

        cv::Mat gry_mat;
        cv::cvtColor(src_mat, gry_mat, cv::COLOR_BGR2GRAY);

        if(acc_mat.rows != gry_mat.rows || acc_mat.cols != gry_mat.cols)
            acc_mat = cv::Mat(gry_mat.size(), CV_64F);

        cv::accumulateWeighted(gry_mat, acc_mat, 0.75);
        cv::convertScaleAbs(acc_mat, gry_mat);

        _mutex.lock();
        const int det_min = _det_min;
        const int det_max = _det_max;
        _mutex.unlock();

        const int hrz_min = gry_mat.cols * det_min / 100;
        const int vrt_min = gry_mat.rows * det_min / 100;
        const int hrz_max = gry_mat.cols * det_max / 100;
        const int vrt_max = gry_mat.rows * det_max / 100;

        std::vector<cv::Rect> rois;
        classifier.detectMultiScale(gry_mat, rois, 1.1, 3, 0
            , cv::Size(hrz_min,vrt_min), cv::Size(hrz_max,vrt_max));

        if(isHiddenCapture()) emit captured();
        else {
            if(rois.size() > 0)
                cv::rectangle(src_mat, rois.at(0), cv::Scalar(0,255,0), 1);

            QImage img(src_mat.data, src_mat.cols, src_mat.rows
                , src_mat.step, QImage::Format_RGB888);

            emit captured(img.convertToFormat(
                QImage::Format_ARGB32_Premultiplied));
        }

        if(rois.size() > 0) {
            if(isHiddenDetect()) emit detected();
            else {
                cv::Mat roi_mat = src_mat(rois.at(0));

                QImage img(roi_mat.data, roi_mat.cols, roi_mat.rows
                    , roi_mat.step, QImage::Format_RGB888);

                emit detected(img.convertToFormat(
                    QImage::Format_ARGB32_Premultiplied));
            }
        }
    }
}
