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

        //! Get rss list.
        const QList<QHash<QString,QString> > &list() const;

        //! Get request name.
        virtual QString requestName() const;

    public slots:
        //! Send.
        virtual void send();

    protected:
        //! On reply data read.
        virtual bool onReplyDataReady(int http_code, QByteArray &data);

    private:
        QList<QHash<QString,QString> > _list;

};

#endif
