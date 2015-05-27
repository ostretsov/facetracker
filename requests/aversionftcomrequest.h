#ifndef AVERSIONFTCOMREQUEST_H
#define AVERSIONFTCOMREQUEST_H

#include "aftcomrequest.h"

class AVersionFtcomRequest : public AFtcomRequest {
    Q_OBJECT

    signals:
        void version(int value);

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

    protected:
        //! On reply data read.
        virtual bool onReplyDataReady(int http_code, QByteArray &data);

};

#endif
