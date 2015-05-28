#ifndef AREQUEST_H
#define AREQUEST_H

#include <QtCore/QPointer>
#include <QtCore/QObject>

class QNetworkAccessManager;
class QHttpMultiPart;

class ARequest : public QObject {
    Q_OBJECT

    signals:
        void readyRead(const QByteArray &data);
        void succeed();
        void failed();

        void serverTime(const qint64 &ts);

    public:
        //! Constructor.
        explicit ARequest(QObject *parent = NULL);

        //! Constructor.
        explicit ARequest(QNetworkAccessManager *nam, QObject *parent = NULL);

        //! Destructor.
        virtual ~ARequest() {}

        //! Get NAM.
        QNetworkAccessManager *nam() const;

        //! Set NAM.
        void setNam(QNetworkAccessManager *nam);

        //! Get request name.
        virtual QString requestName() const;

    public slots:
        //! GET.
        void get(const QUrl &url);

        //! POST.
        void post(const QUrl &url, QHttpMultiPart *multi_part);

    protected:
        //! On reply data read.
        virtual bool onReplyDataReady(int http_code, QByteArray &data);

    private:
        QPointer<QNetworkAccessManager> _nam;

    private slots:
        //! On reply ready read.
        void onReplyReadyRead();

};

#endif
