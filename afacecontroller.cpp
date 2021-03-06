#include <QtCore/QStateMachine>
#include <QtCore/QFinalState>
#include <QtCore/QTimer>

#include "helpers/asettingshelper.h"

#include "afacecontroller.h"
#include "afacecapturethread.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AFaceController::AFaceController(QObject *parent) : QObject(parent) {
    const int obj_min_size
        = ASettingsHelper::value(QStringLiteral("object-min-size")
            , QVariant(20)).toInt();

    const int obj_max_size
        = ASettingsHelper::value(QStringLiteral("object-max-size")
            , QVariant(50)).toInt();

    _capture = new AFaceCaptureThread(this);
    _capture->setDetectorMinSize(obj_min_size);
    _capture->setDetectorMaxSize(obj_max_size);

    _timer = new QTimer(this);
    _timer->setSingleShot(true);
    _timer->setInterval(5000);

    _machine = new QStateMachine(this);
    connect(_machine, SIGNAL(started()), this, SIGNAL(started()));
    connect(_machine, SIGNAL(stopped()), this, SIGNAL(stopped()));
    connect(_machine, SIGNAL(finished()), this, SIGNAL(finished()));

    QState *initial_state = new QState(_machine);
    connect(initial_state, SIGNAL(entered()), _capture, SLOT(start()));
    connect(initial_state, SIGNAL(entered()), _timer, SLOT(start()));

    QState *detection1_state = new QState(_machine);
    connect(detection1_state, SIGNAL(entered()), _timer, SLOT(start()));

    QState *detection2_state = new QState(_machine);
    connect(detection2_state, SIGNAL(entered()), _timer, SLOT(start()));

    QState *fail_state = new QState(_machine);
    connect(fail_state, SIGNAL(entered()), this, SLOT(stop())
        , Qt::QueuedConnection);

    QFinalState *face_in_state = new QFinalState(_machine);
    connect(face_in_state, SIGNAL(entered()), _timer, SLOT(stop()));
    connect(face_in_state, SIGNAL(entered())
        , this, SLOT(stopCapturing()), Qt::QueuedConnection);
    connect(face_in_state, SIGNAL(entered())
        , this, SIGNAL(faceIn()), Qt::QueuedConnection);

    QFinalState *face_out_state = new QFinalState(_machine);
    connect(face_out_state, SIGNAL(entered()), _timer, SLOT(stop()));
    connect(face_out_state, SIGNAL(entered())
        , this, SLOT(stopCapturing()), Qt::QueuedConnection);
    connect(face_out_state, SIGNAL(entered())
        , this, SIGNAL(faceOut()), Qt::QueuedConnection);

    initial_state->addTransition(_capture
        , SIGNAL(started()), detection1_state);
    initial_state->addTransition(_capture
        , SIGNAL(finished()), fail_state);
    initial_state->addTransition(_timer
        , SIGNAL(timeout()), face_out_state);

    detection1_state->addTransition(_capture
        , SIGNAL(detected(const QRect&)), detection2_state);
    detection1_state->addTransition(_capture
        , SIGNAL(failed()), fail_state);
    detection1_state->addTransition(_timer
        , SIGNAL(timeout()), face_out_state);

    detection2_state->addTransition(_capture
        , SIGNAL(detected(const QRect&)), face_in_state);
    detection2_state->addTransition(_capture
        , SIGNAL(failed()), fail_state);
    detection2_state->addTransition(_timer
        , SIGNAL(timeout()), face_out_state);

    _machine->setInitialState(initial_state);
}


// ========================================================================== //
// Destructor.
// ========================================================================== //
AFaceController::~AFaceController() {stopCapturing();}


// ========================================================================== //
// Get detect duration.
// ========================================================================== //
int AFaceController::detectDuration() const {return _timer->interval();}


// ========================================================================== //
// Get is running.
// ========================================================================== //
bool AFaceController::isRunning() const {return _machine->isRunning();}


// ========================================================================== //
// Set detect duration.
// ========================================================================== //
void AFaceController::setDetectDuration(int value) {_timer->setInterval(value);}


// ========================================================================== //
// Start.
// ========================================================================== //
void AFaceController::start() {_machine->start();}


// ========================================================================== //
// Stop.
// ========================================================================== //
void AFaceController::stop() {
    stopCapturing(); _timer->stop(); _machine->stop();
}


// ========================================================================== //
// Stop capturing.
// ========================================================================== //
void AFaceController::stopCapturing() {
    if(_capture->isRunning()) {
        _capture->requestInterruption();
        _capture->wait();
    }
}
