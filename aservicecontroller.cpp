#include <QtCore/QCoreApplication>
#include <QtCore/QGlobalStatic>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include <QtWidgets/QMenu>

#include "database/atablecontroller.h"

#include "systemtrayicon/asystemtrayicon.h"

#include "aservicecontroller.h"
#include "aservicemetatypecontroller.h"
#include "aservicedatabasecontroller.h"
#include "asettingsdialog.h"

Q_GLOBAL_STATIC(AServiceController, _g_service_ctrl)

// ========================================================================== //
// Convert message type to char.
// ========================================================================== //
QChar messageTypeToChar(QtMsgType type) {
    switch(type) {
        case QtDebugMsg:    return QChar('I');
        case QtWarningMsg:  return QChar('W');
        case QtCriticalMsg: return QChar('C');
        case QtFatalMsg:    return QChar('F');
    }

    return QChar();
}


// ========================================================================== //
// Handle message.
// ========================================================================== //
void handleMessage(QtMsgType type, const QMessageLogContext &ctx
    , const QString &msg) {

    if(AServiceController::instance()->isDatabaseOpened()) {
        ATableController *messages = AServiceController::instance()->messages();
        if(messages) {
            const qint64 msecs = QDateTime::currentMSecsSinceEpoch();

            const QString fname = QFileInfo(ctx.file).fileName();

            QVariantHash hash;
            hash.insert(QStringLiteral("datetime"), msecs);
            hash.insert(QStringLiteral("type")    , messageTypeToChar(type));
            hash.insert(QStringLiteral("category"), ctx.category);
            hash.insert(QStringLiteral("text")    , msg);
            hash.insert(QStringLiteral("file")    , fname);
            hash.insert(QStringLiteral("function"), ctx.function);
            hash.insert(QStringLiteral("line")    , ctx.line);

            if(messages->appendRow(hash)) {
                QMetaObject::invokeMethod(messages, "submitAll"
                    , Qt::QueuedConnection);

                if(type == QtFatalMsg) abort();

                return;
            }
        }
    }

    const QDateTime dt = QDateTime::currentDateTime();

    const QString text
        = QString("[%1][%2] %3").arg(messageTypeToChar(type))
            .arg(dt.toString(QStringLiteral("hh:mm:ss"))).arg(msg);

    const QString dname
        = QCoreApplication::applicationDirPath() + QLatin1String("/logs");

    QDir dir(dname); if(!dir.exists()) dir.mkpath(dname);

    const QString fname
        = QString("%1/logs/%2.log")
            .arg(QCoreApplication::applicationDirPath())
            .arg(dt.toString(QStringLiteral("yyyy_MM_dd")));

    QFile file(fname);
    if(file.open(QFile::WriteOnly|QFile::Append|QFile::Text)) {
        QTextStream stream(&file);

        #ifdef Q_OS_WIN
            stream.setCodec("Windows-1251");
        #endif

        stream << text << endl;

        file.close();
    }

    if(type == QtFatalMsg) abort();
}


// ========================================================================== //
// Get instance.
// ========================================================================== //
AServiceController *AServiceController::instance() {return _g_service_ctrl;}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AServiceController::AServiceController(QObject *parent)
    : QObject(parent), _service_db_ctrl(new AServiceDatabaseController(this)) {

    qInstallMessageHandler(handleMessage);

    AServiceMetatypeController::registerMetaTypes();

    _service_db_ctrl->openConnection();

    createTray();

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()));

// TODO: for tests only, remove later.
ASettingsDialog dlg;
dlg.exec();
QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
//
}


// ========================================================================== //
// Get database is opened.
// ========================================================================== //
bool AServiceController::isDatabaseOpened() const {
    return _service_db_ctrl->isOpened();
}


// ========================================================================== //
// Get messages.
// ========================================================================== //
ATableController *AServiceController::messages() const {
    return _service_db_ctrl->messages();
}


// ========================================================================== //
// Get statistic.
// ========================================================================== //
ATableController *AServiceController::statistic() const {
    return _service_db_ctrl->statistic();
}


// ========================================================================== //
// Get rss.
// ========================================================================== //
ATableController *AServiceController::rss() const {
    return _service_db_ctrl->rss();
}


// ========================================================================== //
// Create tray.
// ========================================================================== //
void AServiceController::createTray() {
    const QString app_name
        = qApp->applicationName() + QLatin1String(" v")
            + qApp->applicationVersion();

    QMenu *tray_menu = new QMenu(app_name);
    tray_menu->addAction(AServiceController::tr("Quit"), qApp, SLOT(quit()));

    _tray = ASystemTrayIcon::create(this);
    _tray->setIcon(QIcon(QStringLiteral(":/images/gray.png")));
    _tray->setContextMenu(tray_menu);
    _tray->show();
}


// ========================================================================== //
// Shutdown.
// ========================================================================== //
void AServiceController::shutdown() {
    if(_tray && _tray->contextMenu())
        delete _tray->contextMenu();

    if(_service_db_ctrl->isOpened())
        _service_db_ctrl->closeConnection();
}
