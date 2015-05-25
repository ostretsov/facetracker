#ifndef AUNIFORMSYSTEMTRAYICON_H
#define AUNIFORMSYSTEMTRAYICON_H

#include "asystemtrayicon.h"

class QSystemTrayIcon;

class AUniformSystemTrayIcon : public ASystemTrayIcon {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AUniformSystemTrayIcon(QObject *parent = NULL);

        //! Destructor.
        virtual ~AUniformSystemTrayIcon() {}

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
        QSystemTrayIcon *_tray_icon;

};

#endif
