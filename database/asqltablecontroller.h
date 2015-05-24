#ifndef ASQLTABLECONTROLLER_H
#define ASQLTABLECONTROLLER_H

#include <QtCore/QQueue>

#include "atablecontroller.h"

class ASqlTableController : public ATableController {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ASqlTableController(QObject *parent = NULL);

        //! Constructor.
        explicit ASqlTableController(const QString &conn_name
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~ASqlTableController() {}

        //! Get connection name.
        const QString &connectionName() const;

        //! Set connection name.
        void setConnectionName(const QString &name);

        //! Get primary field name.
        virtual QString primaryFieldName() const = 0;

        //! Get field index.
        virtual int fieldIndex(const QString &field_name) const;

        //! Append row.
        virtual bool appendRow(const QVariantHash &hash);

        //! Get data.
        virtual QVariant data(int key_id, const QString &field_name) const;

        //! Set data.
        virtual bool setData(int key_id, const QString &field_name
            , const QVariant &value);

    public slots:
        //! Select.
        virtual bool select();

        //! Submit.
        virtual bool submit();

        //! Submit all.
        virtual bool submitAll();

        //! Clear all.
        virtual bool clearAll();

    private:
        QString _conn_name;

        QQueue<QVariantHash> _rows;

    private slots:
        //! Append queued row.
        void appendQueuedRow();

};

#endif
