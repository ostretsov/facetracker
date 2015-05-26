#ifndef AFTCOMREQUEST_H
#define AFTCOMREQUEST_H

#include "arequest.h"

class AFtcomRequest : public ARequest {
    Q_OBJECT

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

    private:
        QString _domain, _locale;

};

#endif
