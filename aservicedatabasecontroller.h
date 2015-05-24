#ifndef ASERVICEDATABASECONTROLLER_H
#define ASERVICEDATABASECONTROLLER_H

#include <QtCore/QPointer>

#include "database/adatabasecontroller.h"

class AMessagesSqlTableController;
class ATableController;

class AServiceDatabaseController : public ADatabaseController {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AServiceDatabaseController(QObject *parent = NULL);

        //! Destructor.
        virtual ~AServiceDatabaseController() {}

        //! Get messages.
        ATableController *messages() const;

    public slots:
        //! Open connection.
        virtual bool openConnection();

        //! Close connection.
        virtual void closeConnection();

    private:
        QPointer<AMessagesSqlTableController> _messages_tbl_ctrl;

};

#endif
