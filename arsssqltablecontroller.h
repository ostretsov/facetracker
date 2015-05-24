#ifndef ARSSSQLTABLECONTROLLER_H
#define ARSSSQLTABLECONTROLLER_H

#include <QtCore/QPointer>

#include "database/asqltablecontroller.h"

class QSqlTableModel;

class ARssSqlTableController : public ASqlTableController {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ARssSqlTableController(QObject *parent = NULL);

        //! Constructor.
        explicit ARssSqlTableController(const QString &conn_name
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~ARssSqlTableController() {}

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
