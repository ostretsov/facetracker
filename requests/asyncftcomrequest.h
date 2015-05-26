#ifndef ASYNCFTCOMREQUEST_H
#define ASYNCFTCOMREQUEST_H

#include "aftcomrequest.h"

class ASyncFtcomRequest : public AFtcomRequest {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ASyncFtcomRequest(QObject *parent = NULL);

        //! Constructor.
        explicit ASyncFtcomRequest(QNetworkAccessManager *nam
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~ASyncFtcomRequest() {}

        //! Get data.
        const QList<QHash<QString,QByteArray> > &data() const;

        //! Set data.
        void setData(const QList<QHash<QString,QByteArray> > &data);

        //! Append data item.
        void appendDataItem(const QHash<QString,QByteArray> &hash);

    public slots:
        //! Send.
        virtual void send();

    private:
        QList<QHash<QString,QByteArray> > _data;

};

#endif
