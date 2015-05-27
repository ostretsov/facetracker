#include <QtCore/QCoreApplication>
#include <QtCore/QGlobalStatic>
#include <QtCore/QTranslator>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include <QtNetwork/QNetworkAccessManager>

#include <QtWidgets/QMenu>

#include "database/atablecontroller.h"

#include "requests/alogoutftcomrequest.h"
#include "requests/aloginftcomrequest.h"

#include "systemtrayicon/asystemtrayicon.h"

#include "helpers/asettingshelper.h"

#include "aservicecontroller.h"
#include "aservicemetatypecontroller.h"
#include "aservicedatabasecontroller.h"
#include "asessioncontroller.h"
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

    if(QLatin1String(ctx.category) == QLatin1String("app"))
        AServiceController::instance()->showMessage(msg);

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
    : QObject(parent), _mode(MODE_GRAY), _authorized(false)
    , _service_db_ctrl(new AServiceDatabaseController(this))
    , _session_ctrl(new ASessionController(this))
    , _nam(new QNetworkAccessManager(this)), _qt_tr(NULL), _app_tr(NULL) {

    //qInstallMessageHandler(handleMessage);

    AServiceMetatypeController::registerMetaTypes();

    installTranslator();

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()));

    _service_db_ctrl->openConnection();

    connect(_session_ctrl, SIGNAL(grayActivated())
        , this, SLOT(onGrayActivated()));
    connect(_session_ctrl, SIGNAL(greenActivated())
        , this, SLOT(onGreenActivated()));
    connect(_session_ctrl, SIGNAL(redActivated())
        , this, SLOT(onRedActivated()));

    createTray();

    QMetaObject::invokeMethod(this, "showSettingsDialog", Qt::QueuedConnection);
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
// Get is authorized.
// ========================================================================== //
bool AServiceController::isAuthorized() const {return _authorized;}


// ========================================================================== //
// Install translator.
// ========================================================================== //
void AServiceController::installTranslator() {
    const QString locale
        = ASettingsHelper::value(QStringLiteral("locale")
            , QVariant(QStringLiteral("en"))).toString();

    if(locale == QLatin1String("en")) {
        if(_qt_tr) qApp->removeTranslator(_qt_tr);
        if(_app_tr) qApp->removeTranslator(_app_tr);

    } else {
        if(_qt_tr) qApp->removeTranslator(_qt_tr);
        else _qt_tr = new QTranslator(this);

        if(_app_tr) qApp->removeTranslator(_app_tr);
        else _app_tr = new QTranslator(this);

        const QString path
            = qApp->applicationDirPath() + QLatin1String("/translations");

        if(_qt_tr->load(QLatin1String("qt_")+locale, path))
            qApp->installTranslator(_qt_tr);

        if(_app_tr->load(qApp->applicationName()+QLatin1Char('_')+locale, path))
            qApp->installTranslator(_app_tr);
    }

    emit trChanged();
}


// ========================================================================== //
// Login.
// ========================================================================== //
void AServiceController::login() {
    emit loginStarted();

    const QString domain
        = ASettingsHelper::value(QStringLiteral("domain")
            , QVariant(QStringLiteral("face-tracker.com"))).toString();

    const QString locale
        = ASettingsHelper::value(QStringLiteral("locale")
            , QVariant(QStringLiteral("en"))).toString();

    const QString username
        = ASettingsHelper::value(QStringLiteral("username")).toString();
    const QString password
        = ASettingsHelper::value(QStringLiteral("password")).toString();

    ALoginFtcomRequest *request = new ALoginFtcomRequest(this);
    request->setNam(_nam);
    request->setDomain(domain);
    request->setLocale(locale);
    request->setUsername(username);
    request->setPassword(password);

    connect(request, &ALoginFtcomRequest::serverTime
        , [this](const qint64 &ts) {
        _session_ctrl->setRemoteDeltaTimeStamp(
            QDateTime::currentMSecsSinceEpoch()-ts);
    });

    connect(request, &ALoginFtcomRequest::message
        , [this](const QString &msg) {showMessage(msg);});

    connect(request, &ALoginFtcomRequest::succeed, [this,request]() {
        _authorized = true; emit loginSucceed(); request->deleteLater();
    });

    connect(request, &ALoginFtcomRequest::failed, [this,request]() {
        _authorized = false; emit loginFailed(); request->deleteLater();
    });

    request->send();
}


// ========================================================================== //
// Logout.
// ========================================================================== //
void AServiceController::logout() {
    emit logoutStarted();

    ALogoutFtcomRequest *request = new ALogoutFtcomRequest(this);
    request->setNam(_nam);
    request->setDomain(QStringLiteral("face-tracker.com"));
    request->setLocale(QStringLiteral("en"));

    connect(request, &ALogoutFtcomRequest::serverTime
        , [this](const qint64 &ts) {
        _session_ctrl->setRemoteDeltaTimeStamp(
            QDateTime::currentMSecsSinceEpoch()-ts);
    });

    connect(request, &ALoginFtcomRequest::message
        , [this](const QString &msg) {showMessage(msg);});

    connect(request, &ALogoutFtcomRequest::succeed, [this,request]() {
        _authorized = false; emit logoutSucceed(); request->deleteLater();
    });

    connect(request, &ALogoutFtcomRequest::failed, [this,request]() {
        _authorized = false; emit logoutFailed(); request->deleteLater();
    });

    request->send();
}


// ========================================================================== //
// Start.
// ========================================================================== //
void AServiceController::start() {
    if(_authorized && !_session_ctrl->isRunning())
        _session_ctrl->start();
}


// ========================================================================== //
// Stop.
// ========================================================================== //
void AServiceController::stop() {
    if(_session_ctrl->isRunning())
        _session_ctrl->stop();
}


// ========================================================================== //
// Show message.
// ========================================================================== //
void AServiceController::showMessage(const QString &msg) {
    if(_tray) _tray->showMessage(qApp->applicationName(), msg);
}


// ========================================================================== //
// Create tray.
// ========================================================================== //
void AServiceController::createTray() {
    _settings_action = new QAction(this);
    _settings_action->setText(AServiceController::tr("Settings..."));
    connect(_settings_action, SIGNAL(triggered())
        , this, SLOT(showSettingsDialog()));

    const QString app_name
        = qApp->applicationName() + QLatin1String(" v")
            + qApp->applicationVersion();

    QMenu *tray_menu = new QMenu(app_name);
    tray_menu->addAction(_settings_action);
    tray_menu->addSeparator();
    tray_menu->addAction(AServiceController::tr("Quit"), qApp, SLOT(quit()));

    _tray = ASystemTrayIcon::create(this);
    _tray->setIcon(QStringLiteral(":/images/gray.png"));
    _tray->setContextMenu(tray_menu);
    _tray->show();
}


// ========================================================================== //
// Set actions enabled.
// ========================================================================== //
void AServiceController::setActionsEnabled(bool enabled) {
    _settings_action->setEnabled(enabled);
}


// ========================================================================== //
// Show settings dialog.
// ========================================================================== //
void AServiceController::showSettingsDialog() {
    setActionsEnabled(false);

    stop(); ASettingsDialog().exec(); start();

    setActionsEnabled(true);
}


// ========================================================================== //
// Shutdown.
// ========================================================================== //
void AServiceController::shutdown() {
    if(_tray && _tray->contextMenu())
        delete _tray->contextMenu();

    if(_session_ctrl->isRunning())
        _session_ctrl->stop();

    if(_nam) delete _nam;

    if(_service_db_ctrl->isOpened())
        _service_db_ctrl->closeConnection();
}


// ========================================================================== //
// On gray activated.
// ========================================================================== //
void AServiceController::onGrayActivated() {
    if(_mode == MODE_GRAY) return;

    if(_tray) _tray->setIcon(QStringLiteral(":/images/gray.png"));

    _mode = MODE_GRAY;
}


// ========================================================================== //
// On green activated.
// ========================================================================== //
void AServiceController::onGreenActivated() {
    if(_mode == MODE_GREEN) return;

    if(_tray) _tray->setIcon(QStringLiteral(":/images/green.png"));

    _mode = MODE_GREEN;
}


// ========================================================================== //
// On red activated.
// ========================================================================== //
void AServiceController::onRedActivated() {
    if(_tray) {
        if(_mode != MODE_RED)
            _tray->setIcon(QStringLiteral(":/images/red.png"));

        _tray->showMessage(qApp->applicationName()
            , AServiceController::tr("It is time to afford to rest!"));
    }

    _mode = MODE_RED;
}
