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
bool ARequest::onReplyDataReady(QByteArray &data) {
    emit readyRead(data); return true;
}


// ========================================================================== //
// On reply ready read.
// ========================================================================== //
void ARequest::onReplyReadyRead() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply) return;

    switch(onReplyDataReady(reply->readAll())) {
        case true:  emit succeed(); break;
        case false: emit failed();  break;
    }

    reply->deleteLater();
}
