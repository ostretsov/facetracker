#include "aservicedatabasecontroller.h"
#include "amessagessqltablecontroller.h"

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
// Open connection.
// ========================================================================== //
bool AServiceDatabaseController::openConnection() {
    if(ADatabaseController::openConnection()) {
        _messages_tbl_ctrl = new AMessagesSqlTableController(this);
        _messages_tbl_ctrl->setConnectionName(connectionName());
        if(!_messages_tbl_ctrl->select()) {closeConnection(); return false;}

        return true;
    }

    return false;
}


// ========================================================================== //
// Close connection.
// ========================================================================== //
void AServiceDatabaseController::closeConnection() {
    if(_messages_tbl_ctrl) delete _messages_tbl_ctrl;

    ADatabaseController::closeConnection();
}
