#include <QtCore/QAbstractTableModel>
#include <QtCore/QCoreApplication>
#include <QtCore/QGlobalStatic>
#include <QtCore/QTranslator>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include <QtNetwork/QNetworkAccessManager>

#include <QtWidgets/QMenu>

#include "database/atablecontroller.h"

#include "requests/aversionftcomrequest.h"
#include "requests/alogoutftcomrequest.h"
#include "requests/aloginftcomrequest.h"
#include "requests/asyncftcomrequest.h"
#include "requests/arssftcomrequest.h"

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
    , _sync_interval_counter(0)
    , _service_db_ctrl(new AServiceDatabaseController(this))
    , _session_ctrl(new ASessionController(this))
    , _nam(new QNetworkAccessManager(this))
    , _one_min_timer(new QTimer(this))
    , _qt_tr(NULL), _app_tr(NULL) {

    //qInstallMessageHandler(handleMessage);

    AServiceMetatypeController::registerMetaTypes();

    createActions(); installTranslator(); createTray();

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()));

    _service_db_ctrl->openConnection();

    connect(_session_ctrl, SIGNAL(grayActivated())
        , this, SLOT(onGrayActivated()));
    connect(_session_ctrl, SIGNAL(greenActivated())
        , this, SLOT(onGreenActivated()));
    connect(_session_ctrl, SIGNAL(redActivated())
        , this, SLOT(onRedActivated()));

    _one_min_timer->setSingleShot(false);
    _one_min_timer->setInterval(60000);
    connect(_one_min_timer, SIGNAL(timeout()), this, SLOT(onOneMinTimeout()));

    QMetaObject::invokeMethod(this, "showSettingsDialog", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "checkVersion", Qt::QueuedConnection);
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

    _settings_action->setText(AServiceController::tr("Settings..."));
    _quit_action->setText(AServiceController::tr("Quit"));

    if(_tray) {
        if(_tray->contextMenu())
            delete _tray->contextMenu();

        delete _tray;

        createTray();
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

    const QString domain
        = ASettingsHelper::value(QStringLiteral("domain")
            , QVariant(QStringLiteral("face-tracker.com"))).toString();

    const QString locale
        = ASettingsHelper::value(QStringLiteral("locale")
            , QVariant(QStringLiteral("en"))).toString();

    ALogoutFtcomRequest *request = new ALogoutFtcomRequest(this);
    request->setNam(_nam);
    request->setDomain(domain);
    request->setLocale(locale);

    connect(request, &ALogoutFtcomRequest::serverTime
        , [this](const qint64 &ts) {
        _session_ctrl->setRemoteDeltaTimeStamp(
            QDateTime::currentMSecsSinceEpoch()-ts);
    });

    connect(request, &ALogoutFtcomRequest::message
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
// Sync.
// ========================================================================== //
void AServiceController::sync() {
    emit syncStarted();

    const QString domain
        = ASettingsHelper::value(QStringLiteral("domain")
            , QVariant(QStringLiteral("face-tracker.com"))).toString();

    const QString locale
        = ASettingsHelper::value(QStringLiteral("locale")
            , QVariant(QStringLiteral("en"))).toString();

    ASyncFtcomRequest *request = new ASyncFtcomRequest(this);
    request->setNam(_nam);
    request->setDomain(domain);
    request->setLocale(locale);

    connect(request, &ASyncFtcomRequest::serverTime
        , [this](const qint64 &ts) {
        _session_ctrl->setRemoteDeltaTimeStamp(
            QDateTime::currentMSecsSinceEpoch()-ts);
    });

    connect(request, &ASyncFtcomRequest::message
        , [this](const QString &msg) {showMessage(msg);});

    connect(request, &ASyncFtcomRequest::succeed, [this,request]() {
        emit syncSucceed(); request->deleteLater();
    });

    connect(request, &ASyncFtcomRequest::failed, [this,request]() {
        emit syncFailed(); request->deleteLater();
    });

    const int period_from_fidx
        = statistic()->fieldIndex(QStringLiteral("period_from"));
    const int period_to_fidx
        = statistic()->fieldIndex(QStringLiteral("period_to"));
    const int is_extra_time_fidx
        = statistic()->fieldIndex(QStringLiteral("is_extra_time"));
    const int is_synced_fidx
        = statistic()->fieldIndex(QStringLiteral("is_synced"));

    QAbstractItemModel *model = statistic()->model();
    for(int row = 0, rows = model->rowCount(); row < rows; ++row) {
        QModelIndex is_synced_index = model->index(row,is_synced_fidx);
        if(is_synced_index.isValid()
            && model->data(is_synced_index).toInt() == 0) {

            QModelIndex period_from_index
                = model->index(row,period_from_fidx);
            if(!period_from_index.isValid()) continue;

            QModelIndex period_to_index
                = model->index(row,period_to_fidx);
            if(!period_to_index.isValid()) continue;

            QModelIndex is_extra_time_index
                = model->index(row,is_extra_time_fidx);
            if(!is_extra_time_index.isValid()) continue;

            QHash<QString,QByteArray> hash;

            hash.insert(QStringLiteral("period_from")
                , QByteArray::number(model->data(period_from_index)
                    .toLongLong()));

            hash.insert(QStringLiteral("period_to")
                , QByteArray::number(model->data(period_to_index)
                    .toLongLong()));

            if(model->data(is_extra_time_index).toInt() == 1)
                hash.insert(QStringLiteral("is_extra_time"), "on");

            model->setData(is_synced_index, 1);

            request->appendDataItem(hash);
        }
    }

    if(!request->data().isEmpty()) request->send();
    else {emit syncSucceed(); request->deleteLater();}

    QMetaObject::invokeMethod(statistic(), "submitAll", Qt::QueuedConnection);
}


// ========================================================================== //
// Check version.
// ========================================================================== //
void AServiceController::checkVersion() {
    emit versionStarted();

    const qint64 version_last_check
         = ASettingsHelper::value(QStringLiteral("version-last-check")
            , QVariant(0)).toLongLong();

    if(QDateTime::fromMSecsSinceEpoch(version_last_check)
        .daysTo(QDateTime::currentDateTime()) < 7) {

        emit versionSucceed(); return;
    }

    const QString domain
        = ASettingsHelper::value(QStringLiteral("domain")
            , QVariant(QStringLiteral("face-tracker.com"))).toString();

    const QString locale
        = ASettingsHelper::value(QStringLiteral("locale")
            , QVariant(QStringLiteral("en"))).toString();

    AVersionFtcomRequest *request = new AVersionFtcomRequest(this);
    request->setNam(_nam);
    request->setDomain(domain);
    request->setLocale(locale);

    connect(request, &AVersionFtcomRequest::serverTime
        , [this](const qint64 &ts) {
        _session_ctrl->setRemoteDeltaTimeStamp(
            QDateTime::currentMSecsSinceEpoch()-ts);
    });

    connect(request, &AVersionFtcomRequest::message
        , [this](const QString &msg) {showMessage(msg);});

    connect(request, &AVersionFtcomRequest::version
        , [this](int new_ver) {

        ASettingsHelper::setValue(QStringLiteral("version-last-check")
            , QVariant(QDateTime::currentMSecsSinceEpoch()));

        const int cur_ver
            = ASettingsHelper::value(QStringLiteral("version")
                , QVariant(1)).toInt();

        if(cur_ver < new_ver) {
            ASettingsHelper::setValue(QStringLiteral("version")
                , QVariant(new_ver));

            showMessage(AServiceController::tr(
                "Available new application version %1!").arg(new_ver));
        }
    });

    connect(request, &AVersionFtcomRequest::succeed, [this,request]() {
        emit versionSucceed(); request->deleteLater();
    });

    connect(request, &AVersionFtcomRequest::failed, [this,request]() {
        emit versionFailed(); request->deleteLater();
    });

    request->send();
}


// ========================================================================== //
// Check rss.
// ========================================================================== //
void AServiceController::checkRss() {
    emit rssStarted();

    const qint64 rss_last_check
         = ASettingsHelper::value(QStringLiteral("rss-last-check")
            , QVariant(0)).toLongLong();

    if(QDateTime::fromMSecsSinceEpoch(rss_last_check)
        .daysTo(QDateTime::currentDateTime()) < 7) {

        emit rssSucceed(); return;
    }

    const QString domain
        = ASettingsHelper::value(QStringLiteral("domain")
            , QVariant(QStringLiteral("face-tracker.com"))).toString();

    const QString locale
        = ASettingsHelper::value(QStringLiteral("locale")
            , QVariant(QStringLiteral("en"))).toString();

    ARssFtcomRequest *request = new ARssFtcomRequest(this);
    request->setNam(_nam);
    request->setDomain(domain);
    request->setLocale(locale);

    connect(request, &ARssFtcomRequest::succeed, [this,request]() {
        qint64 ts = QDateTime::currentMSecsSinceEpoch();

        ASettingsHelper::setValue(QStringLiteral("rss-last-check")
            , QVariant(ts));

        const QList<QHash<QString,QString> > &list = request->list();
        if(list.isEmpty()) {emit rssSucceed(); request->deleteLater(); return;}

        const int url_fidx = rss()->fieldIndex(QStringLiteral("url"));

        QAbstractItemModel *model = rss()->model();

        QListIterator<QHash<QString,QString> > iter(list);
        while(iter.hasNext()) {
            const QHash<QString,QString> &hash = iter.next();
            if(!hash.contains(QStringLiteral("url"))) continue;

            QModelIndexList indexes
                = model->match(model->index(0,url_fidx), Qt::DisplayRole
                    , hash.value(QStringLiteral("url"))
                    , 1, Qt::MatchFixedString);

            if(!indexes.isEmpty()) continue;

            QVariantHash hash2;
            hash2.insert(QStringLiteral("datetime"), ts+1);
            hash2.insert(QStringLiteral("title")
                , hash.value(QStringLiteral("title")));
            hash2.insert(QStringLiteral("body")
                , hash.value(QStringLiteral("body")));
            hash2.insert(QStringLiteral("url")
                , hash.value(QStringLiteral("url")));

            rss()->appendRow(hash2);
        }

        QMetaObject::invokeMethod(rss(), "submitAll", Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, "rssSucceed", Qt::QueuedConnection);

        request->deleteLater();
    });

    connect(request, &ARssFtcomRequest::failed, [this,request]() {
        emit rssFailed(); request->deleteLater();
    });

    request->send();
}


// ========================================================================== //
// Start.
// ========================================================================== //
void AServiceController::start() {
    if(!_authorized) return;

    sync();

    const qint64 working_period
         = ASettingsHelper::value(QStringLiteral("working-period")
            , QVariant(30)).toLongLong();

    const int web_cam_request_freq
         = ASettingsHelper::value(QStringLiteral("web-cam-request-freq")
            , QVariant(60)).toInt();

    _session_ctrl->setWorkingPeriod(working_period * 60000);
    _session_ctrl->setWebcamRequestFreq(web_cam_request_freq*1000);

    if(!_session_ctrl->isRunning())
        _session_ctrl->start();

    _sync_interval_counter = 0;

    _one_min_timer->start();
}


// ========================================================================== //
// Stop.
// ========================================================================== //
void AServiceController::stop() {
    _one_min_timer->stop();

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
// Create actions.
// ========================================================================== //
void AServiceController::createActions() {
    _settings_action = new QAction(this);
    _settings_action->setText(AServiceController::tr("Settings..."));
    connect(_settings_action, SIGNAL(triggered())
        , this, SLOT(showSettingsDialog()));

    _quit_action = new QAction(this);
    _quit_action->setText(AServiceController::tr("Quit"));
    connect(_quit_action, SIGNAL(triggered()), qApp, SLOT(quit()));
}


// ========================================================================== //
// Create tray.
// ========================================================================== //
void AServiceController::createTray() {
    const QString app_name
        = qApp->applicationName() + QLatin1String(" v")
            + qApp->applicationVersion();

    QMenu *tray_menu = new QMenu(app_name);
    tray_menu->addAction(_settings_action);
    tray_menu->addSeparator();
    tray_menu->addAction(_quit_action);

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
// On one minute timeout.
// ========================================================================== //
void AServiceController::onOneMinTimeout() {
    ++_sync_interval_counter;

    if(ASettingsHelper::value(QStringLiteral("sync-freq"), QVariant(30)).toInt()
        <= _sync_interval_counter) {sync(); _sync_interval_counter = 0;}
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
