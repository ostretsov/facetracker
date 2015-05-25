#include <QtCore/QStateMachine>
#include <QtCore/QFinalState>
#include <QtCore/QTimer>

#include "asessioncontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ASessionController::ASessionController(QObject *parent) : QObject(parent) {
    _timer = new QTimer(this);
    _timer->setSingleShot(true);
    _timer->setInterval(10000);

    _machine = new QStateMachine(this);
    connect(_machine, SIGNAL(started()), this, SIGNAL(started()));
    connect(_machine, SIGNAL(stopped()), this, SIGNAL(stopped()));
    connect(_machine, SIGNAL(finished()), this, SIGNAL(finished()));

    QState *initial_state = new QState(_machine);

    _machine->setInitialState(initial_state);
}


// ========================================================================== //
// Get is running.
// ========================================================================== //
bool ASessionController::isRunning() const {return _machine->isRunning();}


// ========================================================================== //
// Start.
// ========================================================================== //
void ASessionController::start() {_machine->start();}


// ========================================================================== //
// Stop.
// ========================================================================== //
void ASessionController::stop() {_timer->stop(); _machine->stop();}
