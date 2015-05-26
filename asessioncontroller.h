#ifndef ASESSIONCONTROLLER_H
#define ASESSIONCONTROLLER_H

#include <QtCore/QObject>
#include <QtCore/QPair>

class QStateMachine;
class QTimer;

class AFaceController;

class ASessionController : public QObject {
    Q_OBJECT

    signals:
        void started();
        void stopped();
        void finished();

        void grayActivated();
        void greenActivated();
        void redActivated();

    public:
        //! Constructor.
        explicit ASessionController(QObject *parent = NULL);

        //! Destructor.
        virtual ~ASessionController() {}

        //! Get remote time stamp.
        qint64 remoteTimeStamp() const;

        //! Get working period.
        qint64 workingPeriod() const;

        //! Get webcam request frequency.
        int webcamRequestFreq() const;

        //! Get is running.
        bool isRunning() const;

    public slots:
        //! Set remote time stamp.
        void setRemoteTimeStamp(const qint64 &ts);

        //! Set working period.
        void setWorkingPeriod(const qint64 &ms);

        //! Set webcam request frequency.
        void setWebcamRequestFreq(int ms);

        //! Start.
        void start();

        //! Stop.
        void stop();

    private:
        qint64 _remote_ts, _working_period;

        AFaceController *_face_ctrl;

        QTimer *_timer;

        QStateMachine *_machine;

        QList<QPair<qint64,bool> > _detections;

    private slots:
        //! On face in.
        void onFaceIn();

        //! On face out.
        void onFaceOut();

};

#endif
