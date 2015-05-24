#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

#include "adatabasecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ADatabaseController::ADatabaseController(QObject *parent)
    : QObject(parent), _conn_name(QCoreApplication::applicationName()) {}


// ========================================================================== //
// Get connection name.
// ========================================================================== //
const QString &ADatabaseController::connectionName() const {return _conn_name;}


// ========================================================================== //
// Get database opened.
// ========================================================================== //
bool ADatabaseController::isOpened() const {
    if(!QSqlDatabase::contains(_conn_name)) return false;
    return QSqlDatabase::database(_conn_name,false).isOpen();
}


// ========================================================================== //
// Open connection.
// ========================================================================== //
bool ADatabaseController::openConnection() {
    QSqlDatabase db
        = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), _conn_name);

    if(!db.isValid()) {
        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "database").warning()
            << qPrintable(db.lastError().text());

        emit connectionFailed();

        return false;
    }

    db.setDatabaseName(QCoreApplication::applicationDirPath() + QLatin1Char('/')
        + _conn_name + QLatin1String(".sqlite"));

    if(!db.open()) {
        QMessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "database").warning()
            << qPrintable(db.lastError().text());

        emit connectionFailed();

        return false;
    }

    emit connectionOpened();

    return true;
}


// ========================================================================== //
// Close connection.
// ========================================================================== //
void ADatabaseController::closeConnection() {
    if(isOpened()) {closeDatabase(); QSqlDatabase::removeDatabase(_conn_name);}

    emit connectionClosed();
}


// ========================================================================== //
// Close database.
// ========================================================================== //
void ADatabaseController::closeDatabase() {
    QSqlDatabase db = QSqlDatabase::database(_conn_name,false);
    if(db.isValid() && db.isOpen()) db.close();
}
