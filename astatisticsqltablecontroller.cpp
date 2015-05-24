#include <QtCore/QDebug>

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "astatisticsqltablecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AStatisticSqlTableController::AStatisticSqlTableController(QObject *parent)
    : ASqlTableController(parent) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AStatisticSqlTableController::AStatisticSqlTableController(
    const QString &conn_name, QObject *parent)
    : ASqlTableController(conn_name, parent) {}


// ========================================================================== //
// Get model.
// ========================================================================== //
QAbstractItemModel *AStatisticSqlTableController::model() const {return _model;}


// ========================================================================== //
// Get field names.
// ========================================================================== //
QList<QString> AStatisticSqlTableController::fieldNames() const {
    QList<QString> field_names;
    field_names
        << QStringLiteral("statistic_id")
        << QStringLiteral("username")
        << QStringLiteral("session_id")
        << QStringLiteral("fixed_from")
        << QStringLiteral("fixed_to")
        << QStringLiteral("is_extra_time")
        << QStringLiteral("is_synced");

    return field_names;
}


// ========================================================================== //
// Get primary field name.
// ========================================================================== //
QString AStatisticSqlTableController::primaryFieldName() const {
    return QStringLiteral("statistic_id");
}


// ========================================================================== //
// Select.
// ========================================================================== //
bool AStatisticSqlTableController::select() {
    if(!_model) {
        QSqlDatabase db = QSqlDatabase::database(connectionName());
        if(!db.tables().contains(QStringLiteral("statistic"))) {
            const QString query_str
                = QLatin1String("CREATE TABLE IF NOT EXISTS statistic(" \
                    "statistic_id INTEGER PRIMARY KEY AUTOINCREMENT" \
                    ", username TEXT NOT NULL" \
                    ", session_id TEXT NOT NULL" \
                    ", fixed_from INTEGER NOT NULL" \
                    ", fixed_to INTEGER NOT NULL" \
                    ", is_extra_time INTEGER NOT NULL" \
                    ", is_synced INTEGER NOT NULL)");

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
        _model->setTable(QStringLiteral("statistic"));
        _model->setEditStrategy(QSqlTableModel::OnManualSubmit);

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("statistic_id"))
            , Qt::Horizontal, AStatisticSqlTableController::tr("ID"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("username"))
            , Qt::Horizontal, AStatisticSqlTableController::tr("Username"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("session_id"))
            , Qt::Horizontal, AStatisticSqlTableController::tr("Session ID"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("fixed_from"))
            , Qt::Horizontal, AStatisticSqlTableController::tr("Fixed from"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("fixed_to"))
            , Qt::Horizontal, AStatisticSqlTableController::tr("Fixed to"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("is_extra_time"))
            , Qt::Horizontal
            , AStatisticSqlTableController::tr("Is extra time"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("is_synced"))
            , Qt::Horizontal, AStatisticSqlTableController::tr("Is synced"));
    }

    return _model->select();
}
