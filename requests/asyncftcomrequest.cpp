#include <QtNetwork/QHttpMultiPart>

#include "asyncftcomrequest.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ASyncFtcomRequest::ASyncFtcomRequest(QObject *parent) : AFtcomRequest(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
ASyncFtcomRequest::ASyncFtcomRequest(QNetworkAccessManager *nam
    , QObject *parent) : AFtcomRequest(nam, parent) {}


// ========================================================================== //
// Get data.
// ========================================================================== //
const QList<QHash<QString,QByteArray> > &ASyncFtcomRequest::data() const {
    return _data;
}


// ========================================================================== //
// Set data.
// ========================================================================== //
void ASyncFtcomRequest::setData(const QList<QHash<QString,QByteArray> > &data) {
    _data = data;
}


// ========================================================================== //
// Append data item.
// ========================================================================== //
void ASyncFtcomRequest::appendDataItem(const QHash<QString,QByteArray> &hash) {
    _data.append(hash);
}


// ========================================================================== //
// Get request name.
// ========================================================================== //
QString ASyncFtcomRequest::requestName() const {
    return QStringLiteral("SyncFtcomRequest");
}


// ========================================================================== //
// Send.
// ========================================================================== //
void ASyncFtcomRequest::send() {
    QHttpMultiPart *multi_part
        = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    for(int i = 0, n = _data.size(); i < n; ++i) {
        QHashIterator<QString,QByteArray> iter(_data.at(i));
        while(iter.hasNext()) {
            iter.next();

            QHttpPart hash_part;
            hash_part.setHeader(QNetworkRequest::ContentDispositionHeader
                , QString("form-data; name=\"periods[%1][%2]\"")
                .arg(i).arg(iter.key()));
            hash_part.setBody(iter.value());

            multi_part->append(hash_part);
        }
    }

    const QString url
        = QString("http://%1/%2/api/sync").arg(domain()).arg(locale());

    post(QUrl::fromUserInput(url), multi_part);
}
