#include <QtCore/QAbstractTableModel>

#include <QtWidgets/QScrollArea>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include "database/atablecontroller.h"

#include "widgets/aimagewidget.h"

#include "helpers/asettingshelper.h"

#include "asettingsdialog.h"
#include "aservicecontroller.h"
#include "afacecapturethread.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ASettingsDialog::ASettingsDialog(QWidget *parent)
    : QDialog(parent), _capture(new AFaceCaptureThread(this)) {

    setWindowTitle(ASettingsDialog::tr("Settings"));

    _capture->setCaptureHidden(false);

    AImageWidget *img_wdg = new AImageWidget(this);
    img_wdg->setMinimumSize(160,120);
    connect(_capture, SIGNAL(captured(const QImage&))
        , img_wdg, SLOT(updateImage(const QImage&)));
    connect(_capture, SIGNAL(detected(const QRect&))
        , img_wdg, SLOT(updateRoi(const QRect&)));

    QMetaObject::invokeMethod(_capture, "start", Qt::QueuedConnection);

    QLabel *lang_label = new QLabel(this);
    lang_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    lang_label->setText(ASettingsDialog::tr("Language:"));

    const QString locale
        = ASettingsHelper::value(QStringLiteral("locale")
            , QVariant(QStringLiteral("en"))).toString();

    _lang_cbox = new QComboBox(this);
    _lang_cbox->addItem(QStringLiteral("Eng"), QStringLiteral("en"));
    _lang_cbox->addItem(QStringLiteral("Rus"), QStringLiteral("ru"));
    _lang_cbox->setCurrentIndex(_lang_cbox->findData(locale));

    QLabel *user_label = new QLabel(this);
    user_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    user_label->setText(ASettingsDialog::tr("Username:"));

    _user_ledit = new QLineEdit(this);

    QLabel *pswd_label = new QLabel(this);
    pswd_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    pswd_label->setText(ASettingsDialog::tr("Password:"));

    _pswd_ledit = new QLineEdit(this);
    _pswd_ledit->setEchoMode(QLineEdit::Password);

    QLabel *working_period_label = new QLabel(this);
    working_period_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    working_period_label->setText(
        ASettingsDialog::tr("No more in front\nof webcamera:"));

    _working_period_spbox = new QSpinBox(this);
    _working_period_spbox->setSuffix(ASettingsDialog::tr(" min."));
    _working_period_spbox->setRange(1,60);
    _working_period_spbox->setValue(
        ASettingsHelper::value(QStringLiteral("working-period"), 30).toInt());

    const QString registration_link
        = ASettingsHelper::value(QStringLiteral("registration-link"))
            .toString();

    _register_label = new QLabel(this);
    _register_label->setOpenExternalLinks(true);
    _register_label->setText(
        QString("<a href=\"%1\">%2</a>")
            .arg(registration_link)
            .arg(ASettingsDialog::tr("Register")));

    _login_pbut = new QPushButton(this);
    connect(_login_pbut, &QPushButton::clicked, [this]() {
        switch(AServiceController::instance()->isAuthorized()) {
            case true:
                QMetaObject::invokeMethod(AServiceController::instance()
                    , "logout", Qt::QueuedConnection);
            break;

            case false:
                ASettingsHelper::setValue(QStringLiteral("username")
                    , QVariant(_user_ledit->text()));
                ASettingsHelper::setValue(QStringLiteral("password")
                    , QVariant(_pswd_ledit->text()));

                QMetaObject::invokeMethod(AServiceController::instance()
                    , "login", Qt::QueuedConnection);
            break;
        }
    });

    const QString statistic_link
        = ASettingsHelper::value(QStringLiteral("statistic-link"))
            .toString();

    QLabel *statistic_label = new QLabel(this);
    statistic_label->setOpenExternalLinks(true);
    statistic_label->setText(
        ASettingsDialog::tr("Statistic available" \
        " <a href=\"%1\">at link</a>.").arg(statistic_link));

    _rss_label = new QLabel(this);
    _rss_label->setAlignment(Qt::AlignTop);
    _rss_label->setWordWrap(true);
    _rss_label->setOpenExternalLinks(true);
    QMetaObject::invokeMethod(AServiceController::instance()
        , "checkRss", Qt::QueuedConnection);

    QScrollArea *rss_area = new QScrollArea(this);
    rss_area->setWidgetResizable(true);
    rss_area->setWidget(_rss_label);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(img_wdg, 0, 0, 6, 1);
    layout->addWidget(lang_label, 0, 1, 1, 1);
    layout->addWidget(_lang_cbox, 0, 2, 1, 2);
    layout->addWidget(user_label, 1, 1, 1, 1);
    layout->addWidget(_user_ledit, 1, 2, 1, 2);
    layout->addWidget(pswd_label, 2, 1, 1, 1);
    layout->addWidget(_pswd_ledit, 2, 2, 1, 2);
    layout->addWidget(working_period_label, 3, 1, 1, 1);
    layout->addWidget(_working_period_spbox, 3, 2, 1, 1);
    layout->addWidget(_register_label, 1, 4, 1, 1);
    layout->addWidget(_login_pbut, 2, 4, 1, 1);
    layout->addWidget(statistic_label, 4, 2, 1, 3);
    layout->addWidget(rss_area, 5, 1, 1, 4);
    layout->setColumnStretch(3,2);

    setLayout(layout);

    connect(AServiceController::instance(), &AServiceController::loginStarted
        , this, &ASettingsDialog::onLogInOutStarted);
    connect(AServiceController::instance(), &AServiceController::loginSucceed
        , this, &ASettingsDialog::onLogInOutFinished);
    connect(AServiceController::instance(), &AServiceController::loginFailed
        , this, &ASettingsDialog::onLogInOutFinished);

    connect(AServiceController::instance(), &AServiceController::logoutStarted
        , this, &ASettingsDialog::onLogInOutStarted);
    connect(AServiceController::instance(), &AServiceController::logoutSucceed
        , this, &ASettingsDialog::onLogInOutFinished);
    connect(AServiceController::instance(), &AServiceController::logoutFailed
        , this, &ASettingsDialog::onLogInOutFinished);

    connect(AServiceController::instance(), &AServiceController::rssSucceed
        , this, &ASettingsDialog::onRssSucceed);

    const QString username
        = ASettingsHelper::value(QStringLiteral("username")).toString();
    const QString password
        = ASettingsHelper::value(QStringLiteral("password")).toString();

    _user_ledit->setText(username);
    _pswd_ledit->setText(password);

    switch(AServiceController::instance()->isAuthorized()) {
        case true:
            _login_pbut->setText(ASettingsDialog::tr("Logout"));
            setAuthWidgetsEnabled(false);
        break;

        case false:
            _login_pbut->setText(ASettingsDialog::tr("Login"));

            if(!(username.isEmpty() && password.isEmpty())) {
                QMetaObject::invokeMethod(AServiceController::instance()
                    , "login", Qt::QueuedConnection);
            }

        break;
    }

    connect(_lang_cbox
        , static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
        , [this, lang_label, user_label, pswd_label
            , working_period_label, statistic_label](int idx) {

        ASettingsHelper::setValue(QStringLiteral("locale")
            , _lang_cbox->itemData(idx));

        AServiceController::instance()->installTranslator();

        setWindowTitle(ASettingsDialog::tr("Settings"));
        lang_label->setText(ASettingsDialog::tr("Language:"));
        user_label->setText(ASettingsDialog::tr("Username:"));
        pswd_label->setText(ASettingsDialog::tr("Password:"));

        working_period_label->setText(
            ASettingsDialog::tr("No more in front\nof webcamera:"));
        _working_period_spbox->setSuffix(ASettingsDialog::tr(" min."));

        const QString registration_link
            = ASettingsHelper::value(QStringLiteral("registration-link"))
                .toString();

        _register_label->setText(
            QString("<a href=\"%1\">%2</a>")
                .arg(registration_link)
                .arg(ASettingsDialog::tr("Register")));

        switch(AServiceController::instance()->isAuthorized()) {
            case true:
                _login_pbut->setText(ASettingsDialog::tr("Logout"));
            break;

            case false:
                _login_pbut->setText(ASettingsDialog::tr("Login"));
            break;
        }

        const QString statistic_link
            = ASettingsHelper::value(QStringLiteral("statistic-link"))
                .toString();

        statistic_label->setText(
            ASettingsDialog::tr("Statistic available" \
            " <a href=\"%1\">at link</a>.").arg(statistic_link));

        AServiceController::instance()->rss()->clearAll();

        ASettingsHelper::setValue(QStringLiteral("rss-last-check")
            , QVariant(0));

        QMetaObject::invokeMethod(AServiceController::instance()
            , "checkRss", Qt::QueuedConnection);
    });
}


// ========================================================================== //
// Destructor.
// ========================================================================== //
ASettingsDialog::~ASettingsDialog() {
    if(_capture->isRunning()) {
        _capture->requestInterruption();
        _capture->wait();
    }
}


// ========================================================================== //
// Set widgets enabled.
// ========================================================================== //
void ASettingsDialog::setWidgetsEnabled(bool enabled) {
    _lang_cbox->setEnabled(enabled);
    _working_period_spbox->setEnabled(enabled);
    _login_pbut->setEnabled(enabled);
}


// ========================================================================== //
// Set authorization widgets enabled.
// ========================================================================== //
void ASettingsDialog::setAuthWidgetsEnabled(bool enabled) {
    _user_ledit->setEnabled(enabled);
    _pswd_ledit->setEnabled(enabled);
}


// ========================================================================== //
// On log in out started.
// ========================================================================== //
void ASettingsDialog::onLogInOutStarted() {
    setWidgetsEnabled(false); setAuthWidgetsEnabled(false);
}


// ========================================================================== //
// On log in out finished.
// ========================================================================== //
void ASettingsDialog::onLogInOutFinished() {
    setWidgetsEnabled(true);

    switch(AServiceController::instance()->isAuthorized()) {
        case true:
            _login_pbut->setText(ASettingsDialog::tr("Logout"));
            setAuthWidgetsEnabled(false);
        break;

        case false:
            _login_pbut->setText(ASettingsDialog::tr("Login"));
            setAuthWidgetsEnabled(true);
        break;
    }
}


// ========================================================================== //
// On rss succeed.
// ========================================================================== //
void ASettingsDialog::onRssSucceed() {
    QAbstractItemModel *model = AServiceController::instance()->rss()->model();
    if(model->rowCount() == 0) return;

    const int title_fidx
        = AServiceController::instance()->rss()
            ->fieldIndex(QStringLiteral("title"));
    const int body_fidx
        = AServiceController::instance()->rss()
            ->fieldIndex(QStringLiteral("body"));
    const int url_fidx
        = AServiceController::instance()->rss()
            ->fieldIndex(QStringLiteral("url"));

    const QString title = model->data(model->index(0,title_fidx)).toString();
    const QString body  = model->data(model->index(0,body_fidx)).toString();
    const QString url   = model->data(model->index(0,url_fidx)).toString();

    const QString text
        = QString("<h3><a href=\"%1\">%2</a></h3>%3")
            .arg(url).arg(title).arg(body);

    _rss_label->setText(text);
}
