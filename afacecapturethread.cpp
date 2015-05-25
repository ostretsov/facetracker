#include "afacecapturethread.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AFaceCaptureThread::AFaceCaptureThread(QObject *parent)
    : ACaptureThread(parent) {

    setDetectorClassifier(QStringLiteral(
        ":/classifiers/haar_frontalface_alt_tree.xml"));
}
