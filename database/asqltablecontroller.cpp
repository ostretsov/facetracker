#include <QtCore/QDebug>

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "asqltablecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ASqlTableController::ASqlTableController(QObject *parent)
    : ATableController(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
ASqlTableController::ASqlTableController(const QString &conn_name
    , QObject *parent) : ATableController(parent), _conn_name(conn_name) {}


// ========================================================================== //
// Get connection name.
// ========================================================================== //
const QString &ASqlTableController::connectionName() const {return _conn_name;}


// ========================================================================== //
// Set connection name.
// ========================================================================== //
void ASqlTableController::setConnectionName(const QString &name) {
    _conn_name = name;
}


// ========================================================================== //
// Get field index.
// ========================================================================== //
int ASqlTableController::fieldIndex(const QString &field_name) const {
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(this->model());
    return (model) ? model->fieldIndex(field_name) : -1;
}


// ========================================================================== //
// Append row.
// ========================================================================== //
bool ASqlTableController::appendRow(const QVariantHash &hash) {
    if(!model() || hash.isEmpty()) return false;

    _rows.enqueue(hash);

    QMetaObject::invokeMethod(this, "appendQueuedRow", Qt::QueuedConnection);

    return true;
}


// ========================================================================== //
// Get data.
// ========================================================================== //
QVariant ASqlTableController::data(int key_id
    , const QString &field_name) const {

    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(this->model());
    if(!model) return QVariant();

    const int key_id_field = model->fieldIndex(primaryFieldName());
    if(key_id_field == -1) return QVariant();

    QModelIndexList indexes
        = model->match(model->index(0,key_id_field)
            , Qt::DisplayRole, key_id, 1, Qt::MatchFixedString);

    if(indexes.isEmpty()) return QVariant();

    return model->data(model->index(indexes.first().row()
        , model->fieldIndex(field_name)));
}


// ========================================================================== //
// Set data.
// ========================================================================== //
bool ASqlTableController::setData(int key_id, const QString &field_name
    , const QVariant &value) {

    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(this->model());
    if(!model) return false;

    const int key_id_field = model->fieldIndex(primaryFieldName());
    if(key_id_field == -1) return false;

    QModelIndexList indexes
        = model->match(model->index(0,key_id_field)
            , Qt::DisplayRole, key_id, 1, Qt::MatchFixedString);

    if(indexes.isEmpty()) return false;

    return model->setData(model->index(indexes.first().row()
        , model->fieldIndex(field_name)), value);
}


// ========================================================================== //
// Select.
// ========================================================================== //
bool ASqlTableController::select() {
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(this->model());
    return (model) ? model->select() : false;
}


// ========================================================================== //
// Submit.
// ========================================================================== //
bool ASqlTableController::submit() {
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(this->model());
    if(!model) return false;

    if(model->isDirty()) {
        QSqlDatabase db = QSqlDatabase::database(_conn_name);
        if(db.transaction()) {
            if(model->submit()) return db.commit();
            else {db.rollback(); return false;}

        } else return model->submit();
    }

    return true;
}


// ========================================================================== //
// Submit all.
// ========================================================================== //
bool ASqlTableController::submitAll() {
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(this->model());
    if(!model) return false;

    if(model->isDirty()) {
        QSqlDatabase db = QSqlDatabase::database(_conn_name);
        if(db.transaction()) {
            if(model->submitAll()) return db.commit();
            else {db.rollback(); return false;}

        } else return model->submitAll();
    }

    return true;
}


// ========================================================================== //
// Clear all.
// ========================================================================== //
bool ASqlTableController::clearAll() {
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(this->model());
    if(model) {
        QSqlDatabase db = QSqlDatabase::database(_conn_name);
        if(db.tables().contains(model->tableName())) {
            const QString query_str
                = QString("DELETE FROM %1").arg(model->tableName());

            QSqlQuery query(db);
            if(!query.exec(query_str)) {
                QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "database")
                    .warning() << qPrintable(ASqlTableController::tr(
                        "Query: \"%1\"; error: \"%2\"")
                        .arg(query_str).arg(query.lastError().text()));

                return false;
            }

            return model->select();
        }
    }

    return true;
}


// ========================================================================== //
// Append queued row.
// ========================================================================== //
void ASqlTableController::appendQueuedRow() {
    if(_rows.isEmpty()) return;

    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(this->model());
    if(!model) return;

    QList<QString> field_names = fieldNames();

    QVariantHash hash = _rows.dequeue();

    QHashIterator<QString,QVariant> hash_iter(hash);
    while(hash_iter.hasNext()) {
        hash_iter.next();

        if(!field_names.contains(hash_iter.key()))
            field_names.removeOne(hash_iter.key());
    }

    if(field_names.isEmpty()) {
        if(!_rows.isEmpty()) {
            QMetaObject::invokeMethod(this, "appendQueuedRow"
                , Qt::QueuedConnection);
        }

        return;
    }

    QSqlRecord record;

    QListIterator<QString> field_names_iter(field_names);
    while(field_names_iter.hasNext()) {
        const QString &field_name = field_names_iter.next();
        const QVariant field_value = hash.value(field_name);

        QSqlField field(field_name, field_value.type());
        field.setValue(field_value);

        record.append(field);
    }

    model->insertRecord(-1, record);

    if(!_rows.isEmpty()) {
        QMetaObject::invokeMethod(this, "appendQueuedRow"
            , Qt::QueuedConnection);
    }
}
