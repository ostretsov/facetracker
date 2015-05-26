#include <QtCore/QStateMachine>
#include <QtCore/QFinalState>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include "database/atablecontroller.h"

#include "asessioncontroller.h"
#include "aservicecontroller.h"
#include "afacecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ASessionController::ASessionController(QObject *parent)
    : QObject(parent), _remote_delta_ts(0), _working_period(/*30*60000*/20000) {

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
// Get remote delta time stamp.
// ========================================================================== //
qint64 ASessionController::remoteDeltaTimeStamp() const {
    return _remote_delta_ts;
}


// ========================================================================== //
// Get working period.
// ========================================================================== //
qint64 ASessionController::workingPeriod() const {return _working_period;}


// ========================================================================== //
// Get webcam request frequency.
// ========================================================================== //
int ASessionController::webcamRequestFreq() const {return _timer->interval();}


// ========================================================================== //
// Get is running.
// ========================================================================== //
bool ASessionController::isRunning() const {return _machine->isRunning();}


// ========================================================================== //
// Set remote delta time stamp.
// ========================================================================== //
void ASessionController::setRemoteDeltaTimeStamp(const qint64 &ts) {
    _remote_delta_ts = ts;
}


// ========================================================================== //
// Set working period.
// ========================================================================== //
void ASessionController::setWorkingPeriod(const qint64 &ms) {
    _working_period = ms;
}


// ========================================================================== //
// Set webcam request frequency.
// ========================================================================== //
void ASessionController::setWebcamRequestFreq(int ms) {_timer->setInterval(ms);}


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
// Clean detections.
// ========================================================================== //
void ASessionController::cleanDetections() {
    if(_detections.isEmpty()) return;

    while(!_detections.first().second) {
        _detections.removeFirst();
        if(_detections.isEmpty()) return;
    }

    if(_detections.size() < 3) return;

    for(int i = 2, n = _detections.size(); i < n; ++i) {
        if(_detections.at(i-2).second
            && !_detections.at(i-1).second
            && _detections.at(i).second) {

            _detections[i-1].second = true;
        }
    }
}


// ========================================================================== //
// Export detection periods.
// ========================================================================== //
void ASessionController::exportDetectionPeriods() {
    if(_detections.isEmpty()) return;

    QList<QPair<qint64,bool> > period;
    for(int i = 2; i < _detections.size(); ++i) {
        if(!_detections.at(i-1).second && !_detections.at(i).second) {
            for(int ii = 0, nn = i-1; ii < nn; ++ii)
                period.append(_detections.takeFirst());

            break;
        }
    }

    if(period.isEmpty()) return;

    int edge = -1;
    for(int i = 1, n = period.size(); i < n; ++i) {
        if((period.at(i).first-period.first().first) > _working_period) {
            edge = i; break;
        }
    }

    QVariantHash hash;
    hash.insert(QStringLiteral("username"), QStringLiteral("unknown"));
    hash.insert(QStringLiteral("session_id"), QStringLiteral("unknown"));
    hash.insert(QStringLiteral("period_from")
        , period.first().first+_remote_delta_ts);

    if(edge == -1) {
        hash.insert(QStringLiteral("period_to")
            , period.last().first+_remote_delta_ts);

    } else {
        hash.insert(QStringLiteral("period_to")
            , period.at(edge).first+_remote_delta_ts);
    }

    hash.insert(QStringLiteral("is_extra_time"), 0);
    hash.insert(QStringLiteral("is_synced"), 0);

    AServiceController::instance()->statistic()->appendRow(hash);

    if(edge != -1) {
        hash[QStringLiteral("period_from")]
            = period.at(edge).first + _remote_delta_ts;

        hash[QStringLiteral("period_to")]
            = period.last().first + _remote_delta_ts;

        hash[QStringLiteral("is_extra_time")] = 1;

        AServiceController::instance()->statistic()->appendRow(hash);
    }

    QMetaObject::invokeMethod(AServiceController::instance()->statistic()
        , "submitAll", Qt::QueuedConnection);
}


// ========================================================================== //
// On face in.
// ========================================================================== //
void ASessionController::onFaceIn() {
    _detections.append(qMakePair(QDateTime::currentMSecsSinceEpoch(),true));

    cleanDetections();

    if((_detections.last().first-_detections.first().first) > _working_period)
        emit redActivated();
    else emit greenActivated();

    exportDetectionPeriods();
}


// ========================================================================== //
// On face out.
// ========================================================================== //
void ASessionController::onFaceOut() {
    _detections.append(qMakePair(QDateTime::currentMSecsSinceEpoch(),false));
    if(_detections.size() > 1 && !_detections.at(_detections.size()-2).second)
        emit grayActivated();

    cleanDetections(); exportDetectionPeriods();
}
