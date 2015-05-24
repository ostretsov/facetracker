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
    : QThread(parent), _dev_idx(0), _hidden_capture(true) {}


// ========================================================================== //
// Get capture is hidden.
// ========================================================================== //
bool ACaptureThread::isHiddenCapture() {
    QMutexLocker locker(&_mutex); return _hidden_capture;
}


// ========================================================================== //
// Set capture hidden.
// ========================================================================== //
void ACaptureThread::setCaptureHidden(bool hidden) {
    QMutexLocker locker(&_mutex); _hidden_capture = hidden;
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

    while(!isInterruptionRequested()) {
        cv::Mat src_mat;
        capture >> src_mat;

        if(!isHiddenCapture()) {
            QImage img(src_mat.data, src_mat.cols, src_mat.rows
                , src_mat.step, QImage::Format_RGB888);

            emit captured(img.convertToFormat(
                QImage::Format_ARGB32_Premultiplied));
        }
    }
}
