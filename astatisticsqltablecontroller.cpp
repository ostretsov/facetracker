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
        << QStringLiteral("period_from")
        << QStringLiteral("period_to")
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
                    ", username TEXT" \
                    ", period_from INTEGER" \
                    ", period_to INTEGER" \
                    ", is_extra_time INTEGER" \
                    ", is_synced INTEGER)");

            QSqlQuery query(db);
            if(!query.exec(query_str)) {
                QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "database")
                    .warning() << qPrintable(
                        QString("Query: \"%1\"; error: \"%2\"")
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
            _model->fieldIndex(QStringLiteral("period_from"))
            , Qt::Horizontal, AStatisticSqlTableController::tr("Period from"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("period_to"))
            , Qt::Horizontal, AStatisticSqlTableController::tr("Period to"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("is_extra_time"))
            , Qt::Horizontal
            , AStatisticSqlTableController::tr("Is extra time"));

        _model->setHeaderData(
            _model->fieldIndex(QStringLiteral("is_synced"))
            , Qt::Horizontal, AStatisticSqlTableController::tr("Is synced"));

        _model->setSort(_model->fieldIndex(QStringLiteral("period_from"))
            , Qt::DescendingOrder);
    }

    return _model->select();
}
