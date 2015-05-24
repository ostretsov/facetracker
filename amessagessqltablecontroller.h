#ifndef AMESSAGESSQLTABLECONTROLLER_H
#define AMESSAGESSQLTABLECONTROLLER_H

#include <QtCore/QPointer>

#include "database/asqltablecontroller.h"

class QSqlTableModel;

class AMessagesSqlTableController : public ASqlTableController {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AMessagesSqlTableController(QObject *parent = NULL);

        //! Constructor.
        explicit AMessagesSqlTableController(const QString &conn_name
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~AMessagesSqlTableController() {}

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
