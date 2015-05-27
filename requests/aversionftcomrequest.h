#ifndef AVERSIONFTCOMREQUEST_H
#define AVERSIONFTCOMREQUEST_H

#include "aftcomrequest.h"

class AVersionFtcomRequest : public AFtcomRequest {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AVersionFtcomRequest(QObject *parent = NULL);

        //! Constructor.
        explicit AVersionFtcomRequest(QNetworkAccessManager *nam
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~AVersionFtcomRequest() {}

    public slots:
        //! Send.
        virtual void send();

};

#endif
