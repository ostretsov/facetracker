#include <QtCore/QCoreApplication>
#include <QtCore/QGlobalStatic>

#include "aservicecontroller.h"
#include "aservicemetatypecontroller.h"

Q_GLOBAL_STATIC(AServiceController, _g_service_ctrl)

// ========================================================================== //
// Get instance.
// ========================================================================== //
AServiceController *AServiceController::instance() {return _g_service_ctrl;}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AServiceController::AServiceController(QObject *parent) : QObject(parent) {
    AServiceMetatypeController::registerMetaTypes();
}
