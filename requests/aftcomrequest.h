#ifndef AFTCOMREQUEST_H
#define AFTCOMREQUEST_H

#include "arequest.h"

class AFtcomRequest : public ARequest {
    Q_OBJECT

    signals:
        void message(const QString &msg);

    public:
        //! Constructor.
        explicit AFtcomRequest(QObject *parent = NULL);

        //! Constructor.
        explicit AFtcomRequest(QNetworkAccessManager *nam
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~AFtcomRequest() {}

        //! Get domain.
        QString domain() const;

        //! Get locale.
        QString locale() const;

    public slots:
        //! Set domain.
        void setDomain(const QString &domain);

        //! Set locale.
        void setLocale(const QString &locale);

        //! Send.
        virtual void send() = 0;

    protected:
        //! On reply data read.
        virtual bool onReplyDataReady(int http_code, QByteArray &data);

    private:
        QString _domain, _locale;

};

#endif
