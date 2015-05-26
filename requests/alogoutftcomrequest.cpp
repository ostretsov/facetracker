#include <QtCore/QUrl>

#include "alogoutftcomrequest.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ALogoutFtcomRequest::ALogoutFtcomRequest(QObject *parent)
    : AFtcomRequest(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
ALogoutFtcomRequest::ALogoutFtcomRequest(QNetworkAccessManager *nam
    , QObject *parent) : AFtcomRequest(nam, parent) {}


// ========================================================================== //
// Send.
// ========================================================================== //
void ALogoutFtcomRequest::send() {
    const QString url
        = QString("http://%1/%2/api/logout").arg(domain()).arg(locale());

    get(QUrl::fromUserInput(url));
}
