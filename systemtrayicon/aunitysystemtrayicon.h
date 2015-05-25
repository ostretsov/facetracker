#ifndef AUNITYSYSTEMTRAYICON_H
#define AUNITYSYSTEMTRAYICON_H

#undef signals
extern "C" {
    #include <libappindicator/app-indicator.h>
    #include <libnotify/notify.h>
    #include <gtk/gtk.h>
}
#define signals public

#include "asystemtrayicon.h"

class AUnitySystemTrayIcon : public ASystemTrayIcon {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AUnitySystemTrayIcon(QObject *parent = NULL);

        //! Destructor.
        virtual ~AUnitySystemTrayIcon();

        //! Set icon.
        virtual void setIcon(const QIcon &icon);

        //! Set icon.
        virtual void setIcon(const QString &fname);

        //! Get context menu.
        virtual QMenu *contextMenu() const;

        //! Set context menu.
        virtual void setContextMenu(QMenu *menu);

        //! Set tooltip.
        virtual void setToolTip(const QString &tip);

    public slots:
        //! Show the tray icon.
        virtual void show();

        //! Show the tray icon message.
        virtual void showMessage(const QString &title, const QString &msg);

    private:
        AppIndicator *_indicator;

        QMenu *_menu;

};

#endif
