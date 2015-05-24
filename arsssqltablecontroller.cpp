#include <QtCore/QDebug>

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "arsssqltablecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ARssSqlTableController::ARssSqlTableController(QObject *parent)
    : ASqlTableController(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
ARssSqlTableController::ARssSqlTableController(const QString &conn_name
    , QObject *parent) : ASqlTableController(conn_name, parent) {}


// ========================================================================== //
// Get model.
// ========================================================================== //
QAbstractItemModel *ARssSqlTableController::model() const {return _model;}


// ========================================================================== //
// Get field names.
// ========================================================================== //
QList<QString> ARssSqlTableController::fieldNames() const {
    QList<QString> field_names;
    field_names
        << QStringLiteral("rss_id")
        << QStringLiteral("title")
        << QStringLiteral("body")
        << QStringLiteral("url");

    return field_names;
}


// ========================================================================== //
// Get primary field name.
// ========================================================================== //
QString ARssSqlTableController::primaryFieldName() const {
    return QStringLiteral("rss_id");
}


// ========================================================================== //
// Select.
// ========================================================================== //
bool ARssSqlTableController::select() {
    if(!_model) {
        QSqlDatabase db = QSqlDatabase::database(connectionName());
        if(!db.tables().contains(QStringLiteral("rss"))) {
            const QString query_str
                = QLatin1String("CREATE TABLE IF NOT EXISTS rss(" \
                    "rss_id INTEGER PRIMARY KEY AUTOINCREMENT" \
                    ", title TEXT" \
                    ", body TEXT" \
                    ", url TEXT)");

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
        _model->setTable(QStringLiteral("rss"));
        _model->setEditStrategy(QSqlTableModel::OnManualSubmit);

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("rss_id"))
            , Qt::Horizontal, ARssSqlTableController::tr("ID"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("title"))
            , Qt::Horizontal, ARssSqlTableController::tr("Title"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("body"))
            , Qt::Horizontal, ARssSqlTableController::tr("Body"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("url"))
            , Qt::Horizontal, ARssSqlTableController::tr("Url"));
    }

    return _model->select();
}
