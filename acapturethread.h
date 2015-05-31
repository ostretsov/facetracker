#ifndef ACAPTURETHREAD_H
#define ACAPTURETHREAD_H

#include <QtCore/QThread>
#include <QtCore/QMutex>

class ACaptureThread : public QThread {
    Q_OBJECT

    signals:
        void captured();
        void captured(const QImage &img);

        void detected(const QRect &roi);
        void detected(const QImage &img);

        void failed();

    public:
        //! Constructor.
        explicit ACaptureThread(QObject *parent = NULL);

        //! Destructor.
        virtual ~ACaptureThread() {}

        //! Get device name.
        QString deviceName();

        //! Get device input format name.
        QString deviceFormatName();

        //! Get detector classifier file name.
        QString detectorClassifier();

        //! Get detector minimum size.
        int detectorMinSize();

        //! Get detector maximum size.
        int detectorMaxSize();

        //! Get capture is hidden.
        bool isHiddenCapture();

        //! Get detection is hidden.
        bool isHiddenDetect();

    public slots:
        //! Set device name.
        void setDeviceName(const QString &name);

        //! Set device input format name.
        void setDeviceFormatName(const QString &name);

        //! Set detector classifier file name.
        void setDetectorClassifier(const QString &fname);

        //! Set detector minimum size.
        void setDetectorMinSize(int min);

        //! Set detector maximum size.
        void setDetectorMaxSize(int max);

        //! Set capture hidden.
        void setCaptureHidden(bool hidden);

        //! Set detection hidden.
        void setDetectHidden(bool hidden);

    protected:
        //! Run.
        virtual void run();

    private:
        QString _dev_name, _dev_fmt_name;

        QString _det_fname;

        int _det_min, _det_max;

        bool _hidden_capture, _hidden_detect;

        QMutex _mutex;

};

#endif
