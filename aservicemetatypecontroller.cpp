#include <opencv2/core/core.hpp>

#include <QtGui/QImage>

#include "aservicemetatypecontroller.h"

// ========================================================================== //
// Register meta types.
// ========================================================================== //
void AServiceMetatypeController::registerMetaTypes() {
    typedef cv::Mat Matrix;
    qRegisterMetaType<Matrix>("cv::Mat");

    typedef std::vector<Matrix> Matrices;
    qRegisterMetaType<Matrices>("std::vector<cv::Mat>");

    qRegisterMetaType<QImage>("QImage");
}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AServiceMetatypeController::AServiceMetatypeController(QObject *parent)
    : QObject(parent) {}
