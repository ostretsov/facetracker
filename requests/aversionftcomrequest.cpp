#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
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


// ========================================================================== //
// On reply data read.
// ========================================================================== //
bool AVersionFtcomRequest::onReplyDataReady(int http_code, QByteArray &data) {
    if(!data.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if(doc.isObject()) {
            QJsonObject obj = doc.object();
            if(obj.contains(QStringLiteral("version")))
                emit version(obj.value(QStringLiteral("version")).toInt());
        }
    }

    return AFtcomRequest::onReplyDataReady(http_code, data);
}
