#include <QtCore/QUrl>

#include "aversionftcomrequest.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AVersionFtcomRequest::AVersionFtcomRequest(QObject *parent)
    : AFtcomRequest(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AVersionFtcomRequest::AVersionFtcomRequest(QNetworkAccessManager *nam
    , QObject *parent) : AFtcomRequest(nam, parent) {}


// ========================================================================== //
// Send.
// ========================================================================== //
void AVersionFtcomRequest::send() {
    const QString url
        = QString("http://%1/%2/api/version").arg(domain()).arg(locale());

    get(QUrl::fromUserInput(url));
}
