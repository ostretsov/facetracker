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
    : QObject(parent), _remote_ts(0), _working_period(10000/*30*60000*/) {

    _face_ctrl = new AFaceController(this);
    connect(_face_ctrl, SIGNAL(faceIn()), this, SLOT(onFaceIn()));
    connect(_face_ctrl, SIGNAL(faceOut()), this, SLOT(onFaceOut()));

    _timer = new QTimer(this);
    _timer->setSingleShot(true);
    _timer->setInterval(/*60000*/2000);
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
        , SIGNAL(finished()), work_state);

    _machine->setInitialState(work_state);
}


// ========================================================================== //
// Get remote time stamp.
// ========================================================================== //
qint64 ASessionController::remoteTimeStamp() const {return _remote_ts;}


// ========================================================================== //
// Get working period.
// ========================================================================== //
qint64 ASessionController::workingPeriod() const {return _working_period;}


// ========================================================================== //
// Get is running.
// ========================================================================== //
bool ASessionController::isRunning() const {return _machine->isRunning();}


// ========================================================================== //
// Set remote time stamp.
// ========================================================================== //
void ASessionController::setRemoteTimeStamp(const qint64 &ts) {_remote_ts = ts;}


// ========================================================================== //
// Set working period.
// ========================================================================== //
void ASessionController::setWorkingPeriod(const qint64 &ms) {
    _working_period = ms;
}


// ========================================================================== //
// Start.
// ========================================================================== //
void ASessionController::start() {_machine->start();}


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
    _detections.append(qMakePair(QDateTime::currentMSecsSinceEpoch(),true));

    while(!_detections.first().second) {
        _detections.removeFirst();
        if(_detections.isEmpty()) break;
    }

    if(_detections.size() < 3) return;

    for(int i = 2, n = _detections.size(); i < n; ++i) {
        if(_detections.at(i-2).second
            && !_detections.at(i-1).second
            && _detections.at(i).second) {

            _detections[i-1].second = true;
        }
    }

    if((_detections.last().first-_detections.first().first) > _working_period)
        emit redActivated();
    else emit greenActivated();

    QList<QPair<qint64,bool> > period;
    for(int i = 2; i < _detections.size(); ++i) {
        if(!_detections.at(i-1).second && !_detections.at(i).second) {
            for(int ii = 0, nn = i-1; ii < nn; ++ii)
                period.append(_detections.takeFirst());

            break;
        }
    }

    if(period.isEmpty()) return;

}


// ========================================================================== //
// On face out.
// ========================================================================== //
void ASessionController::onFaceOut() {
    _detections.append(qMakePair(QDateTime::currentMSecsSinceEpoch(),false));
    if(_detections.size() > 1 && !_detections.at(_detections.size()-2).second)
        emit grayActivated();
}
