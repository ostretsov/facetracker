#include <QtCore/QDebug>

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "amessagessqltablecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AMessagesSqlTableController::AMessagesSqlTableController(QObject *parent)
    : ASqlTableController(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AMessagesSqlTableController::AMessagesSqlTableController(
    const QString &conn_name, QObject *parent)
    : ASqlTableController(conn_name, parent) {}


// ========================================================================== //
// Get model.
// ========================================================================== //
QAbstractItemModel *AMessagesSqlTableController::model() const {return _model;}


// ========================================================================== //
// Get field names.
// ========================================================================== //
QList<QString> AMessagesSqlTableController::fieldNames() const {
    QList<QString> field_names;
    field_names
        << QStringLiteral("message_id")
        << QStringLiteral("datetime")
        << QStringLiteral("type")
        << QStringLiteral("category")
        << QStringLiteral("text")
        << QStringLiteral("file")
        << QStringLiteral("function")
        << QStringLiteral("line");

    return field_names;
}


// ========================================================================== //
// Get primary field name.
// ========================================================================== //
QString AMessagesSqlTableController::primaryFieldName() const {
    return QStringLiteral("message_id");
}


// ========================================================================== //
// Select.
// ========================================================================== //
bool AMessagesSqlTableController::select() {
    if(!_model) {
        QSqlDatabase db = QSqlDatabase::database(connectionName());
        if(!db.tables().contains(QStringLiteral("messages"))) {
            const QString query_str
                = QLatin1String("CREATE TABLE IF NOT EXISTS messages(" \
                    "message_id INTEGER PRIMARY KEY AUTOINCREMENT" \
                    ", datetime INTEGER" \
                    ", type TEXT" \
                    ", category TEXT" \
                    ", text TEXT" \
                    ", file TEXT" \
                    ", function TEXT" \
                    ", line INTEGER)");

            QSqlQuery query(db);
            if(!query.exec(query_str)) {
                QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "database")
                    .warning() << qPrintable(ASqlTableController::tr(
                        "Query: \"%1\"; error: \"%2\"")
                        .arg(query_str).arg(query.lastError().text()));

                return false;
            }
        }

        _model = new QSqlTableModel(this, db);
        _model->setTable(QStringLiteral("messages"));
        _model->setEditStrategy(QSqlTableModel::OnManualSubmit);

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("message_id"))
            , Qt::Horizontal, AMessagesSqlTableController::tr("ID"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("datetime"))
            , Qt::Horizontal, AMessagesSqlTableController::tr("Date"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("type"))
            , Qt::Horizontal, AMessagesSqlTableController::tr("Type"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("category"))
            , Qt::Horizontal, AMessagesSqlTableController::tr("Category"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("text"))
            , Qt::Horizontal, AMessagesSqlTableController::tr("Message"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("file"))
            , Qt::Horizontal, AMessagesSqlTableController::tr("File"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("function"))
            , Qt::Horizontal, AMessagesSqlTableController::tr("Function"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("line"))
            , Qt::Horizontal, AMessagesSqlTableController::tr("Line"));

        _model->setFilter(QStringLiteral("category != \"default\""));

        _model->setSort(_model->fieldIndex(QStringLiteral("datetime"))
            , Qt::DescendingOrder);
        _model->setSort(_model->fieldIndex(QStringLiteral("message_id"))
            , Qt::DescendingOrder);
    }

    return _model->select();
}
