#include <QtCore/QUrl>

#include "arssftcomrequest.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ARssFtcomRequest::ARssFtcomRequest(QObject *parent) : AFtcomRequest(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
ARssFtcomRequest::ARssFtcomRequest(QNetworkAccessManager *nam, QObject *parent)
    : AFtcomRequest(nam, parent) {}


// ========================================================================== //
// Send.
// ========================================================================== //
void ARssFtcomRequest::send() {
    const QString url = QString("http://%1/%2/rss").arg(domain()).arg(locale());
    get(QUrl::fromUserInput(url));
}
