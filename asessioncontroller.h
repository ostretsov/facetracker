#ifndef ASESSIONCONTROLLER_H
#define ASESSIONCONTROLLER_H

#include <QtCore/QObject>

class QStateMachine;
class QTimer;

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

        //! Get is running.
        bool isRunning() const;

    public slots:
        //! Start.
        void start();

        //! Stop.
        void stop();

    private:
        QTimer *_timer;

        QStateMachine *_machine;

};

#endif
