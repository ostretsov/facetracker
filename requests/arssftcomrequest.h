#ifndef ARSSFTCOMREQUEST_H
#define ARSSFTCOMREQUEST_H

#include "aftcomrequest.h"

class ARssFtcomRequest : public AFtcomRequest {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ARssFtcomRequest(QObject *parent = NULL);

        //! Constructor.
        explicit ARssFtcomRequest(QNetworkAccessManager *nam
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~ARssFtcomRequest() {}

    public slots:
        //! Send.
        virtual void send();

};

#endif
