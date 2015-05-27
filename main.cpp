#include <QtCore/QLoggingCategory>
#include <QtCore/QTextCodec>

#include <QtWidgets/QApplication>

#include "aservicecontroller.h"

// ========================================================================== //
//
// ========================================================================== //
int main(int argc, char *argv[]) {
    QCoreApplication::setOrganizationName(QStringLiteral("AlexisSoft"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("fl.ru"));
    QCoreApplication::setApplicationName(QStringLiteral("facetracker"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));

    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setProperty("conf"
        , app.applicationDirPath() + QLatin1Char('/')
            + app.applicationName() + QLatin1String(".ini"));

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QLoggingCategory::setFilterRules(
        QStringLiteral("qt.network.ssl.warning=false"));

    AServiceController::instance();

    return app.exec();
}
