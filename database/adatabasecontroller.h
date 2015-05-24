#ifndef ADATABASECONTROLLER_H
#define ADATABASECONTROLLER_H

#include <QtCore/QObject>

class ADatabaseController : public QObject {
    Q_OBJECT

    signals:
        void connectionOpened();
        void connectionClosed();
        void connectionFailed();

    public:
        //! Constructor.
        explicit ADatabaseController(QObject *parent = NULL);

        //! Destructor.
        virtual ~ADatabaseController() {}

        //! Get connection name.
        const QString &connectionName() const;

        //! Get database opened.
        bool isOpened() const;

    public slots:
        //! Open connection.
        virtual bool openConnection();

        //! Close connection.
        virtual void closeConnection();

    private:
        QString _conn_name;

        //! Close database.
        void closeDatabase();

};

#endif
