#include <QtCore/QDebug>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QNetworkReply>

#include "arequest.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ARequest::ARequest(QObject *parent) : QObject(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
ARequest::ARequest(QNetworkAccessManager *nam, QObject *parent)
    : QObject(parent), _nam(nam) {}


// ========================================================================== //
// Get NAM.
// ========================================================================== //
QNetworkAccessManager *ARequest::nam() const {return _nam;}


// ========================================================================== //
// Set NAM.
// ========================================================================== //
void ARequest::setNam(QNetworkAccessManager *nam) {_nam = nam;}


// ========================================================================== //
// Get request name.
// ========================================================================== //
QString ARequest::requestName() const {return QStringLiteral("Request");}


// ========================================================================== //
// GET.
// ========================================================================== //
void ARequest::get(const QUrl &url) {
    if(!_nam) _nam = new QNetworkAccessManager(this);

    QNetworkReply *reply = _nam->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyReadyRead()));
}


// ========================================================================== //
// POST.
// ========================================================================== //
void ARequest::post(const QUrl &url, QHttpMultiPart *multi_part) {
    if(!_nam) _nam = new QNetworkAccessManager(this);

    QNetworkReply *reply = _nam->post(QNetworkRequest(url), multi_part);
    connect(reply, SIGNAL(finished()), this, SLOT(onReplyReadyRead()));

    if(multi_part) multi_part->setParent(reply);
}


// ========================================================================== //
// On reply data read.
// ========================================================================== //
bool ARequest::onReplyDataReady(int http_code, QByteArray &data) {
    Q_UNUSED(http_code); emit readyRead(data); return true;
}


// ========================================================================== //
// On reply ready read.
// ========================================================================== //
void ARequest::onReplyReadyRead() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) return;

    const int http_code
        = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(reply->hasRawHeader("Server-Time")) {
        bool ok = false;
        const qint64 ts = reply->rawHeader("Server-Time").toLongLong(&ok);
        if(ok) emit serverTime(ts * 1000);
    }

    QByteArray data = reply->readAll();
    switch(onReplyDataReady(http_code,data)) {
        case true:
            QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "network").debug()
                << qPrintable(ARequest::tr("Request \"%1\" succeed.")
                    .arg(requestName()));

            emit succeed();
        break;

        case false:
            QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "network").warning()
                << qPrintable(ARequest::tr("Request \"%1\" failed (HTTP %2)!")
                    .arg(requestName()).arg(http_code));

            emit failed();
        break;
    }

    reply->deleteLater();
}
