#ifndef ASETTINGSDIALOG_H
#define ASETTINGSDIALOG_H

#include <QtWidgets/QDialog>

class QPushButton;
class QComboBox;
class QLineEdit;
class QSpinBox;
class QLabel;

class AFaceCaptureThread;

class ASettingsDialog : public QDialog {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ASettingsDialog(QWidget *parent = NULL);

        //! Destructor.
        virtual ~ASettingsDialog();

    private:
        AFaceCaptureThread *_capture;

        QComboBox *_lang_cbox;

        QLineEdit *_user_ledit, *_pswd_ledit;

        QSpinBox *_working_period_spbox;

        QLabel *_register_label;

        QPushButton *_login_pbut;

    private slots:
        //! Load settings.
        void loadSettings();

        //! On log in out started.
        void onLogInOutStarted();

        //! On log in out succeed.
        void onLogInOutSucceed();

        //! On log in out failed.
        void onLogInOutFailed();

};

#endif
