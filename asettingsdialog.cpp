#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

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

    QLabel *lang_label = new QLabel(this);
    lang_label->setText(ASettingsDialog::tr("Language:"));

    _lang_cbox = new QComboBox(this);
    _lang_cbox->addItem(QStringLiteral("Eng"), QStringLiteral("en"));
    _lang_cbox->addItem(QStringLiteral("Rus"), QStringLiteral("ru"));

    QLabel *user_label = new QLabel(this);
    user_label->setText(ASettingsDialog::tr("Username:"));

    _user_ledit = new QLineEdit(this);

    QLabel *pswd_label = new QLabel(this);
    pswd_label->setText(ASettingsDialog::tr("Password:"));

    _pswd_ledit = new QLineEdit(this);
    _pswd_ledit->setEchoMode(QLineEdit::Password);

    QLabel *working_period_label = new QLabel(this);
    working_period_label->setText(
        ASettingsDialog::tr("No more in front\nof webcamera:"));

    _working_period_spbox = new QSpinBox(this);
    _working_period_spbox->setSuffix(ASettingsDialog::tr(" min."));
    _working_period_spbox->setRange(1,60);

    _register_label = new QLabel(this);
    _register_label->setOpenExternalLinks(true);

    QPushButton *login_pbut = new QPushButton(this);
    login_pbut->setText(ASettingsDialog::tr("Login"));

    connect(login_pbut, &QPushButton::clicked, [this]() {
        switch(AServiceController::instance()->isAuthorized()) {
            case true:  AServiceController::instance()->logout(); break;
            case false: AServiceController::instance()->login();  break;
        }
    });

    connect(AServiceController::instance(), &AServiceController::loginStarted
        , [login_pbut]() {login_pbut->setEnabled(false);});

    connect(AServiceController::instance(), &AServiceController::loginFailed
        , [login_pbut]() {login_pbut->setEnabled(true);});

    connect(AServiceController::instance(), &AServiceController::loginSucceed
        , [login_pbut]() {
            login_pbut->setEnabled(true);
            login_pbut->setText(ASettingsDialog::tr("Logout"));
    });

    connect(AServiceController::instance(), &AServiceController::logoutStarted
        , [login_pbut]() {login_pbut->setEnabled(false);});

    connect(AServiceController::instance(), &AServiceController::logoutFailed
        , [login_pbut]() {login_pbut->setEnabled(true);});

    connect(AServiceController::instance(), &AServiceController::logoutSucceed
        , [login_pbut]() {
            login_pbut->setEnabled(true);
            login_pbut->setText(ASettingsDialog::tr("Login"));
    });

    QLabel *statistic_label = new QLabel(this);
    statistic_label->setOpenExternalLinks(true);
    statistic_label->setText(
        ASettingsDialog::tr("Statistic available" \
        " <a href=\"http://google.ru\">at link</a>."));

    AImageWidget *img_wdg = new AImageWidget(this);
    connect(_capture, SIGNAL(captured(const QImage&))
        , img_wdg, SLOT(updateImage(const QImage&)));
    connect(_capture, SIGNAL(detected(const QRect&))
        , img_wdg, SLOT(updateRoi(const QRect&)));

    QLabel *rss_label = new QLabel(this);
    rss_label->setOpenExternalLinks(true);
    rss_label->setText("RSS...");

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(lang_label, 0, 0, 1, 1);
    layout->addWidget(_lang_cbox, 0, 1, 1, 2);
    layout->addWidget(user_label, 1, 0, 1, 1);
    layout->addWidget(_user_ledit, 1, 1, 1, 2);
    layout->addWidget(pswd_label, 2, 0, 1, 1);
    layout->addWidget(_pswd_ledit, 2, 1, 1, 2);
    layout->addWidget(working_period_label, 3, 0, 1, 1);
    layout->addWidget(_working_period_spbox, 3, 1, 1, 1);
    layout->addWidget(_register_label, 1, 3, 1, 1);
    layout->addWidget(login_pbut, 2, 3, 1, 1);
    layout->addWidget(statistic_label, 4, 1, 1, 2);
    layout->addWidget(img_wdg, 5, 0, 1, 4);
    layout->addWidget(rss_label, 6, 0, 1, 4);
    layout->setColumnStretch(2,2);

    setLayout(layout);

    QMetaObject::invokeMethod(this, "loadSettings", Qt::QueuedConnection);
    QMetaObject::invokeMethod(_capture, "start", Qt::QueuedConnection);
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
// Load settings.
// ========================================================================== //
void ASettingsDialog::loadSettings() {
    _lang_cbox->setCurrentText(
        ASettingsHelper::value(QStringLiteral("language")
            , QStringLiteral("Eng")).toString());

    _user_ledit->setText(
        ASettingsHelper::value(QStringLiteral("username")).toString());
    _pswd_ledit->setText(
        ASettingsHelper::value(QStringLiteral("password")).toString());

    _working_period_spbox->setValue(
        ASettingsHelper::value(QStringLiteral("working-period"), 30).toInt());

    _register_label->setText(
        QString("<a href=\"%1\">%2</a>")
            .arg(ASettingsHelper::value(QStringLiteral("registration-link"))
                .toString())
            .arg(ASettingsDialog::tr("Register")));
}
