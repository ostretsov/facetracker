#ifndef ASERVICEDATABASECONTROLLER_H
#define ASERVICEDATABASECONTROLLER_H

#include <QtCore/QPointer>

#include "database/adatabasecontroller.h"

class AStatisticSqlTableController;
class AMessagesSqlTableController;
class ARssSqlTableController;
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

        //! Get statistic.
        ATableController *statistic() const;

        //! Get rss.
        ATableController *rss() const;

    public slots:
        //! Open connection.
        virtual bool openConnection();

        //! Close connection.
        virtual void closeConnection();

    private:
        QPointer<AMessagesSqlTableController> _messages_tbl_ctrl;

        QPointer<AStatisticSqlTableController> _statistic_tbl_ctrl;

        QPointer<ARssSqlTableController> _rss_tbl_ctrl;

};

#endif
