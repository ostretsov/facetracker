#ifndef ASTATISTICSQLTABLECONTROLLER_H
#define ASTATISTICSQLTABLECONTROLLER_H

#include <QtCore/QPointer>

#include "database/asqltablecontroller.h"

class QSqlTableModel;

class AStatisticSqlTableController : public ASqlTableController {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AStatisticSqlTableController(QObject *parent = NULL);

        //! Constructor.
        explicit AStatisticSqlTableController(const QString &conn_name
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~AStatisticSqlTableController() {}

        //! Get model.
        virtual QAbstractItemModel *model() const;

        //! Get field names.
        virtual QList<QString> fieldNames() const;

        //! Get primary field name.
        virtual QString primaryFieldName() const;

    public slots:
        //! Select.
        virtual bool select();

    private:
        QPointer<QSqlTableModel> _model;

};

#endif
