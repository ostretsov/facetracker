#ifndef ACAPTURETHREAD_H
#define ACAPTURETHREAD_H

#include <QtCore/QThread>

class ACaptureThread : public QThread {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ACaptureThread(QObject *parent = NULL);

        //! Destructor.
        virtual ~ACaptureThread() {}

    protected:
        //! Run.
        virtual void run();

};

#endif
