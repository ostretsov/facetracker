#ifndef AFACECONTROLLER_H
#define AFACECONTROLLER_H

#include <QtCore/QObject>

class QStateMachine;
class QTimer;

class AFaceCaptureThread;

class AFaceController : public QObject {
    Q_OBJECT

    signals:
        void started();
        void stopped();
        void finished();

        void faceIn();
        void faceOut();

    public:
        //! Constructor.
        explicit AFaceController(QObject *parent = NULL);

        //! Destructor.
        virtual ~AFaceController();

        //! Get detect duration.
        int detectDuration() const;

        //! Get is running.
        bool isRunning() const;

    public slots:
        //! Set detect duration.
        void setDetectDuration(int value);

        //! Start.
        void start();

        //! Stop.
        void stop();

    private:
        AFaceCaptureThread *_capture;

        QTimer *_timer;

        QStateMachine *_machine;

    private slots:
        //! Stop capturing.
        void stopCapturing();

};

#endif
