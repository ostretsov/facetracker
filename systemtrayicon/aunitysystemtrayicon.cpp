#include <QtCore/QFile>
#include <QtCore/QDir>

#include <QtWidgets/QMenu>

#include "aunitysystemtrayicon.h"

extern "C" {
    void onGtkMenuItemActivated(GtkMenuItem*, gpointer);
    void onGtkMenuItemToggled(GtkCheckMenuItem*, gpointer);
}

// ========================================================================== //
// On gtk menu item activated.
// ========================================================================== //
void onGtkMenuItemActivated(GtkMenuItem *menu_item, gpointer data) {
    Q_UNUSED(menu_item);

    QAction *action = static_cast<QAction*>(data);
    if(action->isEnabled()) action->trigger();
}


// ========================================================================== //
// On gtk menu item toggled.
// ========================================================================== //
void onGtkMenuItemToggled(GtkCheckMenuItem *menu_item, gpointer data) {
    bool checked = gtk_check_menu_item_get_active(menu_item);

    QAction *action = static_cast<QAction*>(data);
    if(action->isEnabled()) action->setChecked(checked);
}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AUnitySystemTrayIcon::AUnitySystemTrayIcon(QObject *parent)
    : ASystemTrayIcon(parent), _menu(NULL) {

    _indicator
        = app_indicator_new("tray", ""
            , APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
}


// ========================================================================== //
// Destructor.
// ========================================================================== //
AUnitySystemTrayIcon::~AUnitySystemTrayIcon() {
    app_indicator_set_status(_indicator, APP_INDICATOR_STATUS_PASSIVE);
    g_object_unref(_indicator);
}


// ========================================================================== //
// Set icon.
// ========================================================================== //
void AUnitySystemTrayIcon::setIcon(const QIcon &icon) {
    const QString tmp_path = QDir::tempPath();

    _tray_fname    = tmp_path + QLatin1String("/tray.png");
    _balloon_fname = tmp_path + QLatin1String("/tray-balloon.png");

    QPixmap pixmap = icon.pixmap(QSize(16,16));
    pixmap.save(_tray_fname, "PNG");

    QPixmap pixmap1 = icon.pixmap(QSize(48,48));
    pixmap1.save(_balloon_fname, "PNG");

    app_indicator_set_icon_theme_path(_indicator
        , tmp_path.toStdString().c_str());
    app_indicator_set_icon(_indicator, "tray");
}


// ========================================================================== //
// Set icon.
// ========================================================================== //
void AUnitySystemTrayIcon::setIcon(const QString &fname) {
    if(!QFile::exists(fname)) return;

    QImage img(fname);
    if(img.isNull()) return;

    const QString tmp_path = QDir::tempPath();

    _tray_fname
        = tmp_path + QLatin1Char('/') + QFileInfo(fname).baseName()
            + QLatin1String(".png");

    if(!QFile::exists(_tray_fname)) {
        QImage tray_img = img.scaled(QSize(16,16), Qt::KeepAspectRatio);
        tray_img.save(_tray_fname);
    }

    _balloon_fname
        = tmp_path + QLatin1Char('/') + QFileInfo(fname).baseName()
            + QLatin1String("-balloon.png");

    if(!QFile::exists(_balloon_fname)) {
        QImage balloon_img = img.scaled(QSize(48,48), Qt::KeepAspectRatio);
        balloon_img.save(_balloon_fname);
    }

    app_indicator_set_icon_theme_path(_indicator
        , tmp_path.toStdString().c_str());
    app_indicator_set_icon(_indicator
        , QFileInfo(fname).baseName().toStdString().c_str());
}


// ========================================================================== //
// Get context menu.
// ========================================================================== //
QMenu *AUnitySystemTrayIcon::contextMenu() const {return _menu;}


// ========================================================================== //
// Set context menu.
// ========================================================================== //
void AUnitySystemTrayIcon::setContextMenu(QMenu *menu) {
    _menu = menu;

    QList<QAction*> actions = menu->actions();
    if(actions.isEmpty()) return;

    GtkWidget *gtk_menu = gtk_menu_new();

    QListIterator<QAction*> actions_iter(actions);
    while(actions_iter.hasNext()) {
        QAction *action = actions_iter.next();

        GtkWidget *gtk_menu_item = NULL;

        if(action->isSeparator()) {
            gtk_menu_item = gtk_separator_menu_item_new();

        } else {
            if(action->isCheckable()) {
                gtk_menu_item
                    = gtk_check_menu_item_new_with_label(
                        action->text().toStdString().c_str());

                gtk_check_menu_item_set_active((GtkCheckMenuItem*)gtk_menu_item
                    , action->isChecked());

                g_signal_connect(gtk_menu_item, "toggled"
                    , G_CALLBACK(onGtkMenuItemToggled), action);

            } else {
                gtk_menu_item
                    = gtk_menu_item_new_with_label(
                        action->text().toStdString().c_str());

                g_signal_connect(gtk_menu_item, "activate"
                    , G_CALLBACK(onGtkMenuItemActivated), action);
            }
        }

        gtk_menu_shell_append(GTK_MENU_SHELL(gtk_menu), gtk_menu_item);
        gtk_widget_show(gtk_menu_item);
    }

    app_indicator_set_menu(_indicator, GTK_MENU(gtk_menu));
}


// ========================================================================== //
// Set tooltip.
// ========================================================================== //
void AUnitySystemTrayIcon::setToolTip(const QString &tip) {Q_UNUSED(tip);}


// ========================================================================== //
// Show the tray icon.
// ========================================================================== //
void AUnitySystemTrayIcon::show() {
    app_indicator_set_status(_indicator, APP_INDICATOR_STATUS_ACTIVE);
}


// ========================================================================== //
// Show the tray icon message.
// ========================================================================== //
void AUnitySystemTrayIcon::showMessage(const QString &title
    , const QString &msg) {

    if(!notify_init("tray")) return;

    NotifyNotification *notify
        = notify_notification_new(title.toStdString().c_str()
            , msg.toStdString().c_str(), _balloon_fname.toStdString().c_str());

    notify_notification_show(notify, NULL);
    notify_uninit();
}
