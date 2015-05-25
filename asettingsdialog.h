#ifndef ASETTINGSDIALOG_H
#define ASETTINGSDIALOG_H

#include <QtWidgets/QDialog>

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

        QSpinBox *_duration_spbox;

        QLabel *_register_label;

    private slots:
        //! Load settings.
        void loadSettings();

};

#endif
