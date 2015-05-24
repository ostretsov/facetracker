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
        QList<ASqlTableController*> ctrls;
        ctrls << new AMessagesSqlTableController(this)
            << new AStatisticSqlTableController(this)
            << new ARssSqlTableController(this);

        foreach(ASqlTableController *ctrl, ctrls) {
            ctrl->setConnectionName(connectionName());
            if(!ctrl->select()) {closeConnection(); return false;}
        }

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
