extern "C" {
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}

#include <opencv2/objdetect.hpp>
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
    : QThread(parent), _dev_name(QStringLiteral("/dev/video0"))
    , _det_min(20), _det_max(50)
    , _hidden_capture(true), _hidden_detect(true) {}


// ========================================================================== //
// Get device name.
// ========================================================================== //
QString ACaptureThread::deviceName() {
    QMutexLocker locker(&_mutex); return _dev_name;
}


// ========================================================================== //
// Get device input format name.
// ========================================================================== //
QString ACaptureThread::deviceFormatName() {
    QMutexLocker locker(&_mutex); return _dev_fmt_name;
}


// ========================================================================== //
// Get detector classifier file name.
// ========================================================================== //
QString ACaptureThread::detectorClassifier() {
    QMutexLocker locker(&_mutex); return _det_fname;
}


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
// Set device name.
// ========================================================================== //
void ACaptureThread::setDeviceName(const QString &name) {
    QMutexLocker locker(&_mutex); _dev_name = name;
}


// ========================================================================== //
// Set device input format name.
// ========================================================================== //
void ACaptureThread::setDeviceFormatName(const QString &name) {
    QMutexLocker locker(&_mutex); _dev_fmt_name = name;
}


// ========================================================================== //
// Set detector classifier file name.
// ========================================================================== //
void ACaptureThread::setDetectorClassifier(const QString &fname) {
    QMutexLocker locker(&_mutex); _det_fname = fname;
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
    const QString dev_fmt_name = _dev_fmt_name;
    _mutex.unlock();

    AVInputFormat *av_inp_fmt = NULL;
    if(!dev_fmt_name.isEmpty()) {
        av_inp_fmt = av_find_input_format(dev_fmt_name.toLatin1());
        if(!av_inp_fmt) {
            QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
                << qPrintable(ACaptureThread::tr("Unable to find input video" \
                    " format \"%1\"!").arg(dev_fmt_name));

            QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

            return;
        }
    }

    _mutex.lock();
    const QString dev_name = _dev_name;
    _mutex.unlock();

    AVFormatContext *av_fmt_ctx;
    if(avformat_open_input(&av_fmt_ctx, dev_name.toLatin1()
        , av_inp_fmt, NULL) < 0) {

        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
            << qPrintable(ACaptureThread::tr("Unable to open video" \
                " device \"%1\"!").arg(dev_name));

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    if(avformat_find_stream_info(av_fmt_ctx, NULL) < 0) {
        avformat_close_input(&av_fmt_ctx);

        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
            << qPrintable(ACaptureThread::tr("Unable to find video stream!"));

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    int vid_strm_idx = -1;
    for(int i = 0, n = av_fmt_ctx->nb_streams; i < n; ++i) {
        if(av_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            vid_strm_idx = i; break;
        }
    }

    if(vid_strm_idx == -1) {
        avformat_close_input(&av_fmt_ctx);

        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
            << qPrintable(ACaptureThread::tr("Unable to find video stream!"));

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    AVStream *av_vid_strm = av_fmt_ctx->streams[vid_strm_idx];

    AVCodec *av_dec = avcodec_find_decoder(av_vid_strm->codec->codec_id);
    if(!av_dec) {
        avformat_close_input(&av_fmt_ctx);

        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
            << qPrintable(ACaptureThread::tr("Unable to find video decoder!"));

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    if(avcodec_open2(av_vid_strm->codec, av_dec, NULL) < 0) {
        avformat_close_input(&av_fmt_ctx);

        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
            << qPrintable(ACaptureThread::tr("Unable to open video decoder!"));

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    AVCodecContext *av_dec_ctx = av_vid_strm->codec;

    SwsContext *av_sws_ctx
        = sws_getCachedContext(NULL
            , av_dec_ctx->width, av_dec_ctx->height, av_dec_ctx->pix_fmt
            , av_dec_ctx->width, av_dec_ctx->height, AV_PIX_FMT_RGB24
            , SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if(!av_sws_ctx) {
        sws_freeContext(av_sws_ctx);
        avcodec_close(av_vid_strm->codec);
        avformat_close_input(&av_fmt_ctx);

        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
            << qPrintable(ACaptureThread::tr("Unable to open cached context!"));

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    AVFrame *av_cap_frm = av_frame_alloc();
    av_cap_frm->format = AV_PIX_FMT_RGB24;
    av_cap_frm->width  = av_dec_ctx->width;
    av_cap_frm->height = av_dec_ctx->height;
    if(av_frame_get_buffer(av_cap_frm, 32) < 0) {
        av_frame_free(&av_cap_frm);
        sws_freeContext(av_sws_ctx);
        avcodec_close(av_vid_strm->codec);
        avformat_close_input(&av_fmt_ctx);

        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
            << qPrintable(ACaptureThread::tr("Unable to open capture frame!"));

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    AVFrame *av_vid_frm = av_frame_alloc();

    _mutex.lock();
    const QString det_fname = _det_fname;
    _mutex.unlock();

    QTemporaryFile *dst_file = QTemporaryFile::createNativeFile(det_fname);
    if(!dst_file) {
        av_frame_free(&av_vid_frm);
        av_frame_free(&av_cap_frm);
        sws_freeContext(av_sws_ctx);
        avcodec_close(av_vid_strm->codec);
        avformat_close_input(&av_fmt_ctx);

        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
            << qPrintable(ACaptureThread::tr("Can not create temporary file!"));

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    const QString dst_fname = QDir::toNativeSeparators(dst_file->fileName());

    cv::CascadeClassifier classifier;
    if(!classifier.load(dst_fname.toStdString())) {
        av_frame_free(&av_vid_frm);
        av_frame_free(&av_cap_frm);
        sws_freeContext(av_sws_ctx);
        avcodec_close(av_vid_strm->codec);
        avformat_close_input(&av_fmt_ctx);

        delete dst_file;

        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "video").warning()
            << qPrintable(ACaptureThread::tr("Load classifier failed!"));

        QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

        return;
    }

    delete dst_file;

    cv::Mat acc_mat, src_mat
        = cv::Mat::zeros(av_dec_ctx->height, av_dec_ctx->width, CV_8UC3);

    AVPacket av_pkt;
    while(av_read_frame(av_fmt_ctx, &av_pkt) >= 0
        && !isInterruptionRequested()) {

        int pkt_rdy = -1;
        if(avcodec_decode_video2(av_dec_ctx, av_vid_frm
            , &pkt_rdy, &av_pkt) < 0) {

            QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "app").warning()
                << qPrintable(ACaptureThread::tr("Capturing failed!"));

            QMetaObject::invokeMethod(this, "failed", Qt::QueuedConnection);

            break;
        }

        if(!pkt_rdy) {av_free_packet(&av_pkt); continue;}

        sws_scale(av_sws_ctx
            , av_vid_frm->data, av_vid_frm->linesize, 0, av_vid_frm->height
            , av_cap_frm->data, av_cap_frm->linesize);

        for(int y = 0, rows = av_dec_ctx->height; y < rows; ++y) {
            for(int x = 0, cols = av_dec_ctx->width; x < cols; ++x) {
                cv::Vec3b &d = src_mat.at<cv::Vec3b>(y,x);
                d[0] = av_cap_frm->data[0][y*av_cap_frm->linesize[0]+x*3+0];
                d[1] = av_cap_frm->data[0][y*av_cap_frm->linesize[0]+x*3+1];
                d[2] = av_cap_frm->data[0][y*av_cap_frm->linesize[0]+x*3+2];
            }
        }

        av_free_packet(&av_pkt);

        cv::Mat gry_mat;
        cv::cvtColor(src_mat, gry_mat, cv::COLOR_RGB2GRAY);

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

        if(isHiddenCapture()) {
            QMetaObject::invokeMethod(this, "captured", Qt::QueuedConnection);

        } else {
            QImage img(src_mat.data, src_mat.cols, src_mat.rows
                , src_mat.step, QImage::Format_RGB888);

            img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);

            QMetaObject::invokeMethod(this, "captured"
                , Qt::QueuedConnection, Q_ARG(QImage,img));
        }

        if(rois.size() > 0) {
            if(isHiddenDetect()) {
                const cv::Rect &roi = rois.at(0);
                QMetaObject::invokeMethod(this, "detected"
                    , Qt::QueuedConnection
                    , Q_ARG(QRect,QRect(roi.x,roi.y,roi.width,roi.height)));

            } else {
                cv::Mat roi_mat = src_mat(rois.at(0));

                QImage img(roi_mat.data, roi_mat.cols, roi_mat.rows
                    , roi_mat.step, QImage::Format_RGB888);

                img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);

                QMetaObject::invokeMethod(this, "detected"
                    , Qt::QueuedConnection, Q_ARG(QImage,img));
            }
        }
    }

    av_frame_free(&av_vid_frm);
    av_frame_free(&av_cap_frm);
    sws_freeContext(av_sws_ctx);
    avcodec_close(av_vid_strm->codec);
    avformat_close_input(&av_fmt_ctx);

    if(!isHiddenCapture()) {
        QMetaObject::invokeMethod(this, "captured"
            , Qt::QueuedConnection, Q_ARG(QImage,QImage()));
    }
}
