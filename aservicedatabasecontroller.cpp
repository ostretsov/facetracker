#include "aservicedatabasecontroller.h"
#include "astatisticsqltablecontroller.h"
#include "amessagessqltablecontroller.h"
#include "arsssqltablecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AServiceDatabaseController::AServiceDatabaseController(QObject *parent)
    : ADatabaseController(parent) {}


// ========================================================================== //
// Get messages.
// ========================================================================== //
ATableController *AServiceDatabaseController::messages() const {
    return _messages_tbl_ctrl;
}


// ========================================================================== //
// Get statistic.
// ========================================================================== //
ATableController *AServiceDatabaseController::statistic() const {
    return _statistic_tbl_ctrl;
}


// ========================================================================== //
// Get rss.
// ========================================================================== //
ATableController *AServiceDatabaseController::rss() const {
    return _rss_tbl_ctrl;
}


// ========================================================================== //
// Open connection.
// ========================================================================== //
bool AServiceDatabaseController::openConnection() {
    if(ADatabaseController::openConnection()) {
        _messages_tbl_ctrl = new AMessagesSqlTableController(this);
        _messages_tbl_ctrl->setConnectionName(connectionName());
        if(!_messages_tbl_ctrl->select()) {closeConnection(); return false;}

        _statistic_tbl_ctrl = new AStatisticSqlTableController(this);
        _statistic_tbl_ctrl->setConnectionName(connectionName());
        if(!_statistic_tbl_ctrl->select()) {closeConnection(); return false;}

        _rss_tbl_ctrl = new ARssSqlTableController(this);
        _rss_tbl_ctrl->setConnectionName(connectionName());
        if(!_rss_tbl_ctrl->select()) {closeConnection(); return false;}

        return true;
    }

    return false;
}


// ========================================================================== //
// Close connection.
// ========================================================================== //
void AServiceDatabaseController::closeConnection() {
    if(_messages_tbl_ctrl)  delete _messages_tbl_ctrl;
    if(_statistic_tbl_ctrl) delete _statistic_tbl_ctrl;
    if(_rss_tbl_ctrl)       delete _rss_tbl_ctrl;

    ADatabaseController::closeConnection();
}
