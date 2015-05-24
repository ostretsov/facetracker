#ifndef ASERVICECONTROLLER_H
#define ASERVICECONTROLLER_H

#include <QtCore/QObject>

class AServiceDatabaseController;
class ATableController;

class AServiceController : public QObject {
    Q_OBJECT

    public:
        //! Get instance.
        static AServiceController *instance();

        //! Constructor.
        explicit AServiceController(QObject *parent = NULL);

        //! Destructor.
        virtual ~AServiceController() {}

        //! Get database is opened.
        bool isDatabaseOpened() const;

        //! Get messages.
        ATableController *messages() const;

    private:
        AServiceDatabaseController *_service_db_ctrl;

};

#endif
