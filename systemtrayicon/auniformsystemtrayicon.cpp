#include <QtWidgets/QSystemTrayIcon>

#include "auniformsystemtrayicon.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AUniformSystemTrayIcon::AUniformSystemTrayIcon(QObject *parent)
    : ASystemTrayIcon(parent), _tray_icon(new QSystemTrayIcon(this)) {}


// ========================================================================== //
// Set icon.
// ========================================================================== //
void AUniformSystemTrayIcon::setIcon(const QIcon &icon) {
    _tray_icon->setIcon(icon);
}


// ========================================================================== //
// Set icon.
// ========================================================================== //
void AUniformSystemTrayIcon::setIcon(const QString &fname) {
    _tray_icon->setIcon(QIcon(fname));
}


// ========================================================================== //
// Get context menu.
// ========================================================================== //
QMenu *AUniformSystemTrayIcon::contextMenu() const {
    return _tray_icon->contextMenu();
}


// ========================================================================== //
// Set context menu.
// ========================================================================== //
void AUniformSystemTrayIcon::setContextMenu(QMenu *menu) {
    _tray_icon->setContextMenu(menu);
}


// ========================================================================== //
// Set tooltip.
// ========================================================================== //
void AUniformSystemTrayIcon::setToolTip(const QString &tip) {
    _tray_icon->setToolTip(tip);
}


// ========================================================================== //
// Show the tray icon.
// ========================================================================== //
void AUniformSystemTrayIcon::show() {_tray_icon->show();}


// ========================================================================== //
// Show the tray icon message.
// ========================================================================== //
void AUniformSystemTrayIcon::showMessage(const QString &title
    , const QString &msg) {

    _tray_icon->showMessage(title, msg);
}
