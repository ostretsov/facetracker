#include "asystemtrayicon.h"
#include "auniformsystemtrayicon.h"

#ifdef Q_OS_LINUX
    #include "aunitysystemtrayicon.h"
#endif

// ========================================================================== //
// Create the tray icon.
// ========================================================================== //
ASystemTrayIcon *ASystemTrayIcon::create(QObject *parent) {
    #ifndef Q_OS_LINUX
        return new AUniformSystemTrayIcon(parent);
    #else
        QString desktop = getenv("XDG_CURRENT_DESKTOP");

        bool is_unity = (desktop.toLower() == QLatin1String("unity"));
        if(!is_unity) return new AUniformSystemTrayIcon(parent);

        return new AUnitySystemTrayIcon(parent);
    #endif
}


// ========================================================================== //
// Constructor.
// ========================================================================== //
ASystemTrayIcon::ASystemTrayIcon(QObject *parent) : QObject(parent) {}
