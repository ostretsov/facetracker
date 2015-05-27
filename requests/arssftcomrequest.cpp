#include <QtCore/QUrl>

#include <QtXml/QDomDocument>

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
// Get rss list.
// ========================================================================== //
const QList<QHash<QString,QString> > &ARssFtcomRequest::list() const {
    return _list;
}


// ========================================================================== //
// Send.
// ========================================================================== //
void ARssFtcomRequest::send() {
    _list.clear();

    const QString url = QString("http://%1/%2/rss").arg(domain()).arg(locale());
    get(QUrl::fromUserInput(url));
}


// ========================================================================== //
// On reply data read.
// ========================================================================== //
bool ARssFtcomRequest::onReplyDataReady(int http_code, QByteArray &data) {
    if(http_code != 200) return false;

    QDomDocument doc;
    if(!doc.setContent(data)) return false;

    QDomNodeList items = doc.elementsByTagName(QStringLiteral("item"));
    if(items.isEmpty()) return true;

    for(int i = 0, n = items.count(); i < n; ++i) {
        QDomElement item = items.at(i).toElement();

        QDomNodeList titles = item.elementsByTagName(QStringLiteral("title"));
        if(titles.isEmpty()) continue;

        QDomNodeList links = item.elementsByTagName(QStringLiteral("link"));
        if(links.isEmpty()) continue;

        QDomNodeList descrs
            = item.elementsByTagName(QStringLiteral("description"));
        if(descrs.isEmpty()) continue;

        QHash<QString,QString> hash;
        hash.insert(QStringLiteral("title"), titles.at(0).toElement().text());
        hash.insert(QStringLiteral("url"), links.at(0).toElement().text());
        hash.insert(QStringLiteral("body"), descrs.at(0).toElement().text());

        _list.append(hash);
    }

    return true;
}
