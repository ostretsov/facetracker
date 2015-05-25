#ifndef ASYSTEMTRAYICON_H
#define ASYSTEMTRAYICON_H

#include <QtCore/QObject>

class QMenu;

class ASystemTrayIcon : public QObject {
    Q_OBJECT

    public:
        //! Create the tray icon.
        static ASystemTrayIcon *create(QObject *parent = NULL);

        //! Constructor.
        explicit ASystemTrayIcon(QObject *parent = NULL);

        //! Destructor.
        virtual ~ASystemTrayIcon() {}

        //! Set icon.
        virtual void setIcon(const QIcon &icon) = 0;

        //! Set icon.
        virtual void setIcon(const QString &fname) = 0;

        //! Get context menu.
        virtual QMenu *contextMenu() const = 0;

        //! Set context menu.
        virtual void setContextMenu(QMenu *menu) = 0;

        //! Set tooltip.
        virtual void setToolTip(const QString &tip) = 0;

    public slots:
        //! Show the tray icon.
        virtual void show() = 0;

        //! Show the tray icon message.
        virtual void showMessage(const QString &title, const QString &msg) = 0;

};

#endif
