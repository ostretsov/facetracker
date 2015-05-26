#include <QtNetwork/QHttpMultiPart>

#include "aloginftcomrequest.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ALoginFtcomRequest::ALoginFtcomRequest(QObject *parent)
    : AFtcomRequest(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
ALoginFtcomRequest::ALoginFtcomRequest(QNetworkAccessManager *nam
    , QObject *parent) : AFtcomRequest(nam, parent) {}


// ========================================================================== //
// Get username.
// ========================================================================== //
QString ALoginFtcomRequest::username() const {return _user;}


// ========================================================================== //
// Get password.
// ========================================================================== //
QString ALoginFtcomRequest::password() const {return _pswd;}


// ========================================================================== //
// Set username.
// ========================================================================== //
void ALoginFtcomRequest::setUsername(const QString &user) {_user = user;}


// ========================================================================== //
// Set password.
// ========================================================================== //
void ALoginFtcomRequest::setPassword(const QString &pswd) {_pswd = pswd;}


// ========================================================================== //
// Send.
// ========================================================================== //
void ALoginFtcomRequest::send() {
    QHttpPart user_part;
    user_part.setHeader(QNetworkRequest::ContentDispositionHeader
        , QString("form-data; name=\"username\""));
    user_part.setBody(_user.toUtf8());

    QHttpPart pswd_part;
    pswd_part.setHeader(QNetworkRequest::ContentDispositionHeader
        , QString("form-data; name=\"password\""));
    pswd_part.setBody(_pswd.toUtf8());

    QHttpMultiPart *multi_part
        = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    multi_part->append(user_part);
    multi_part->append(pswd_part);

    const QString url
        = QString("http://%1/%2/api/login").arg(domain()).arg(locale());

    post(QUrl::fromUserInput(url), multi_part);
}
