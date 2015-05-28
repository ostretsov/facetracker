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
// Get request name.
// ========================================================================== //
QString ALogoutFtcomRequest::requestName() const {
    return QStringLiteral("LogoutFtcomRequest");
}


// ========================================================================== //
// Send.
// ========================================================================== //
void ALogoutFtcomRequest::send() {
    const QString url
        = QString("http://%1/%2/api/logout").arg(domain()).arg(locale());

    get(QUrl::fromUserInput(url));
}
