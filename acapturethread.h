#ifndef ACAPTURETHREAD_H
#define ACAPTURETHREAD_H

#include <QtCore/QThread>
#include <QtCore/QMutex>

class ACaptureThread : public QThread {
    Q_OBJECT

    signals:
        void captured(const QImage &img);

    public:
        //! Constructor.
        explicit ACaptureThread(QObject *parent = NULL);

        //! Destructor.
        virtual ~ACaptureThread() {}

        //! Get capture is hidden.
        bool isHiddenCapture();

    public slots:
        //! Set capture hidden.
        void setCaptureHidden(bool hidden);

    protected:
        //! Run.
        virtual void run();

    private:
        int _dev_idx;

        bool _hidden_capture;

        QMutex _mutex;

};

#endif
