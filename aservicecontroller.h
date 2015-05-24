#ifndef ASERVICECONTROLLER_H
#define ASERVICECONTROLLER_H

#include <QtCore/QObject>

class AServiceDatabaseController;
class ATableController;
class ASystemTrayIcon;

class AServiceController : public QObject {
    Q_OBJECT

    public:
        //! Get instance.
        static AServiceController *instance();

        //! Constructor.
        explicit AServiceController(QObject *parent = NULL);

        //! Destructor.
        virtual ~AServiceController() {}

        //! Get database is opened.
        bool isDatabaseOpened() const;

        //! Get messages.
        ATableController *messages() const;

        //! Get statistic.
        ATableController *statistic() const;

        //! Get rss.
        ATableController *rss() const;

    private:
        AServiceDatabaseController *_service_db_ctrl;

        ASystemTrayIcon *_tray;

        //! Create tray.
        void createTray();

    private slots:
        //! Shutdown.
        void shutdown();

};

#endif
