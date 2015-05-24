#include <QtCore/QTranslator>
#include <QtCore/QTextCodec>
#include <QtCore/QLocale>

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

    QLocale::setDefault(QLocale(QLocale::Russian,QLocale::RussianFederation));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    const QString locale_name = QLocale::system().name();

    QTranslator qt_translator;
    if(qt_translator.load(QLatin1String("qt_") + locale_name
        , app.applicationDirPath() + QLatin1String("/translations")))
        app.installTranslator(&qt_translator);

    QTranslator app_translator;
    if(app_translator.load(QLatin1String("facetracker_") + locale_name
        , app.applicationDirPath() + QLatin1String("/translations")))
        app.installTranslator(&app_translator);

    checkApplicationVersion(app);

    AServiceController::instance();

    return app.exec();
}
