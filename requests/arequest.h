#ifndef AREQUEST_H
#define AREQUEST_H

#include <QtCore/QPointer>
#include <QtCore/QObject>

class QNetworkAccessManager;

class ARequest : public QObject {
    Q_OBJECT

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

        //! Get locale.
        QString locale() const;

        //! Set locale.
        void setLocale(const QString &locale);

    private:
        QPointer<QNetworkAccessManager> _nam;

        QString _locale;

};

#endif
