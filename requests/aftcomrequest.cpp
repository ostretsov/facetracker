#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "aftcomrequest.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AFtcomRequest::AFtcomRequest(QObject *parent) : ARequest(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AFtcomRequest::AFtcomRequest(QNetworkAccessManager *nam, QObject *parent)
    : ARequest(nam, parent) {}


// ========================================================================== //
// Get domain.
// ========================================================================== //
QString AFtcomRequest::domain() const {return _domain;}


// ========================================================================== //
// Get locale.
// ========================================================================== //
QString AFtcomRequest::locale() const {return _locale;}


// ========================================================================== //
// Get request name.
// ========================================================================== //
QString AFtcomRequest::requestName() const {
    return QStringLiteral("FtcomRequest");
}


// ========================================================================== //
// Set domain.
// ========================================================================== //
void AFtcomRequest::setDomain(const QString &domain) {_domain = domain;}


// ========================================================================== //
// Set locale.
// ========================================================================== //
void AFtcomRequest::setLocale(const QString &locale) {_locale = locale;}


// ========================================================================== //
// On reply data read.
// ========================================================================== //
bool AFtcomRequest::onReplyDataReady(int http_code, QByteArray &data) {
    int status_code = -1;

    if(!data.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if(doc.isObject()) {
            QJsonObject obj = doc.object();

            if(obj.contains(QStringLiteral("status")))
                status_code = obj.value(QStringLiteral("status")).toInt();

            if(obj.contains(QStringLiteral("message"))) {
                const QString msg
                    = obj.value(QStringLiteral("message")).toString();

                if(!msg.isEmpty()) emit message(msg);
            }
        }
    }

    return (http_code == 200 && status_code == 0) ? true : false;
}
