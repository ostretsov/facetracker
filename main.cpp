#include <QtCore/QLoggingCategory>
#include <QtCore/QTextCodec>

#include <QtWidgets/QApplication>

#include "helpers/asettingshelper.h"

#include "aservicecontroller.h"

// ========================================================================== //
// Check application version.
// ========================================================================== //
void checkApplicationVersion(const QCoreApplication &app) {
    QStringList versions;
    versions << QStringLiteral("1.0");

    const QString version
        = ASettingsHelper::value(QStringLiteral("version")).toString();

    if(versions.contains(version)) {
        if(app.applicationVersion() != version) {
            ASettingsHelper::setValue(QStringLiteral("version")
                , app.applicationVersion());
        }

        return;
    }

    ASettingsHelper::clear();
    ASettingsHelper::setValue(QStringLiteral("version")
        , app.applicationVersion());
}


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
        , app.applicationDirPath() + QLatin1String("/facetracker.ini"));

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QLoggingCategory::setFilterRules(
        QStringLiteral("qt.network.ssl.warning=false"));

    checkApplicationVersion(app);

    AServiceController::instance();

    return app.exec();
}
