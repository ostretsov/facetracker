#ifndef AFACECAPTURETHREAD_H
#define AFACECAPTURETHREAD_H

#include "acapturethread.h"

class AFaceCaptureThread : public ACaptureThread {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AFaceCaptureThread(QObject *parent = NULL);

        //! Destructor.
        virtual ~AFaceCaptureThread() {}

};

#endif
