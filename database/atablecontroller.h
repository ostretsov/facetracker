#ifndef ATABLECONTROLLER_H
#define ATABLECONTROLLER_H

#include <QtCore/QVariant>
#include <QtCore/QObject>

class QAbstractItemModel;

class ATableController : public QObject {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ATableController(QObject *parent = NULL);

        //! Destructor.
        virtual ~ATableController() {}

        //! Get model.
        virtual QAbstractItemModel *model() const = 0;

        //! Get field names.
        virtual QList<QString> fieldNames() const = 0;

        //! Get field index.
        virtual int fieldIndex(const QString &field_name) const = 0;

        //! Append row.
        virtual bool appendRow(const QVariantHash &hash) = 0;

    public slots:
        //! Submit all.
        virtual bool submitAll() = 0;

        //! Clear all.
        virtual bool clearAll() = 0;

};

#endif
