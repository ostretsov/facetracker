#ifndef ALOGINFTCOMREQUEST_H
#define ALOGINFTCOMREQUEST_H

#include "aftcomrequest.h"

class ALoginFtcomRequest : public AFtcomRequest {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ALoginFtcomRequest(QObject *parent = NULL);

        //! Constructor.
        explicit ALoginFtcomRequest(QNetworkAccessManager *nam
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~ALoginFtcomRequest() {}

        //! Get username.
        QString username() const;

        //! Get password.
        QString password() const;

        //! Get request name.
        virtual QString requestName() const;

    public slots:
        //! Set username.
        void setUsername(const QString &user);

        //! Set password.
        void setPassword(const QString &pswd);

        //! Send.
        virtual void send();

    private:
        QString _user, _pswd;

};

#endif
