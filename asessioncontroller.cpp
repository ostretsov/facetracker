#include <QtCore/QStateMachine>
#include <QtCore/QFinalState>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#include "asessioncontroller.h"
#include "afacecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ASessionController::ASessionController(QObject *parent)
    : QObject(parent), _local_ts(0), _remote_ts(0) {

    _face_ctrl = new AFaceController(this);
    connect(_face_ctrl, SIGNAL(faceIn()), this, SLOT(onFaceIn()));
    connect(_face_ctrl, SIGNAL(faceOut()), this, SLOT(onFaceOut()));

    _timer = new QTimer(this);
    _timer->setSingleShot(true);
    _timer->setInterval(60000);
    connect(_timer, SIGNAL(timeout()), _face_ctrl, SLOT(start()));

    _machine = new QStateMachine(this);
    connect(_machine, SIGNAL(started()), this, SIGNAL(started()));
    connect(_machine, SIGNAL(stopped()), this, SIGNAL(stopped()));
    connect(_machine, SIGNAL(finished()), this, SIGNAL(finished()));

    QState *work_state = new QState(_machine);
    connect(work_state, SIGNAL(entered()), _timer, SLOT(start()));

    QState *fail_state = new QState(_machine);
    connect(fail_state, SIGNAL(entered()), this, SLOT(stop()));

    work_state->addTransition(_face_ctrl
        , SIGNAL(stopped()), fail_state);
    work_state->addTransition(_face_ctrl
        , SIGNAL(faceIn()), work_state);
    work_state->addTransition(_face_ctrl
        , SIGNAL(faceOut()), work_state);

    _machine->setInitialState(work_state);
}


// ========================================================================== //
// Get remote time stamp.
// ========================================================================== //
qint64 ASessionController::remoteTimeStamp() const {return _remote_ts;}


// ========================================================================== //
// Get is running.
// ========================================================================== //
bool ASessionController::isRunning() const {return _machine->isRunning();}


// ========================================================================== //
// Set remote time stamp.
// ========================================================================== //
void ASessionController::setRemoteTimeStamp(const qint64 &ts) {_remote_ts = ts;}


// ========================================================================== //
// Start.
// ========================================================================== //
void ASessionController::start() {
    _local_ts = QDateTime::currentMSecsSinceEpoch();

    _machine->start();
}


// ========================================================================== //
// Stop.
// ========================================================================== //
void ASessionController::stop() {
    _timer->stop(); _machine->stop();

    if(_face_ctrl->isRunning())
        _face_ctrl->stop();
}


// ========================================================================== //
// On face in.
// ========================================================================== //
void ASessionController::onFaceIn() {
    _detections.insert(QDateTime::currentMSecsSinceEpoch() - _local_ts, true);

    if(_detections.size() == 3) {
        QList<qint64> keys = _detections.keys();
        if(_detections.value(keys.at(0))
            && !_detections.value(keys.at(1))
            && _detections.value(keys.at(2))) {

            _detections[keys.at(1)] = true;
        }
    }
}


// ========================================================================== //
// On face out.
// ========================================================================== //
void ASessionController::onFaceOut() {
    _detections.insert(QDateTime::currentMSecsSinceEpoch() - _local_ts, false);
}
