#include <QtCore/QCoreApplication>
#include <QtCore/QGlobalStatic>

#include "aservicecontroller.h"
#include "aservicemetatypecontroller.h"
#include "aservicedatabasecontroller.h"

Q_GLOBAL_STATIC(AServiceController, _g_service_ctrl)

// ========================================================================== //
// Get instance.
// ========================================================================== //
AServiceController *AServiceController::instance() {return _g_service_ctrl;}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AServiceController::AServiceController(QObject *parent)
    : QObject(parent), _service_db_ctrl(new AServiceDatabaseController(this)) {

    AServiceMetatypeController::registerMetaTypes();

    if(_service_db_ctrl->openConnection()) {
        connect(qApp, SIGNAL(aboutToQuit())
            , _service_db_ctrl, SLOT(closeConnection()));
    }
}


// ========================================================================== //
// Get database is opened.
// ========================================================================== //
bool AServiceController::isDatabaseOpened() const {
    return _service_db_ctrl->isOpened();
}


// ========================================================================== //
// Get messages.
// ========================================================================== //
ATableController *AServiceController::messages() const {
    return _service_db_ctrl->messages();
}
