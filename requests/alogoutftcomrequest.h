#ifndef ALOGOUTFTCOMREQUEST_H
#define ALOGOUTFTCOMREQUEST_H

#include "aftcomrequest.h"

class ALogoutFtcomRequest : public AFtcomRequest {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ALogoutFtcomRequest(QObject *parent = NULL);

        //! Constructor.
        explicit ALogoutFtcomRequest(QNetworkAccessManager *nam
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~ALogoutFtcomRequest() {}

        //! Get request name.
        virtual QString requestName() const;

    public slots:
        //! Send.
        virtual void send();

};

#endif
