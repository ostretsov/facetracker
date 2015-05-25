#ifndef ASESSIONCONTROLLER_H
#define ASESSIONCONTROLLER_H

#include <QtCore/QObject>
#include <QtCore/QMap>

class QStateMachine;
class QTimer;

class AFaceController;

class ASessionController : public QObject {
    Q_OBJECT

    signals:
        void started();
        void stopped();
        void finished();

    public:
        //! Constructor.
        explicit ASessionController(QObject *parent = NULL);

        //! Destructor.
        virtual ~ASessionController() {}

        //! Get remote time stamp.
        qint64 remoteTimeStamp() const;

        //! Get is running.
        bool isRunning() const;

    public slots:
        //! Set remote time stamp.
        void setRemoteTimeStamp(const qint64 &ts);

        //! Start.
        void start();

        //! Stop.
        void stop();

    private:
        qint64 _local_ts, _remote_ts;

        AFaceController *_face_ctrl;

        QTimer *_timer;

        QStateMachine *_machine;

        QMap<qint64,bool> _detections;

    private slots:
        //! On face in.
        void onFaceIn();

        //! On face out.
        void onFaceOut();

};

#endif
