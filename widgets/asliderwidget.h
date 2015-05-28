#ifndef ASLIDERWIDGET_H
#define ASLIDERWIDGET_H

#include <QtWidgets/QWidget>

class QSpinBox;
class QSlider;
class QLabel;

class ASliderWidget : public QWidget {
    Q_OBJECT

    signals:
        void valueChanged(int value);

    public:
        //! Constructor.
        explicit ASliderWidget(QWidget *parent = NULL);

        //! Destructor.
        virtual ~ASliderWidget() {}

        //! Get label text.
        QString labelText() const;

        //! Set label text.
        void setLabelText(const QString &text);

        //! Get minimum value.
        int minValue() const;

        //! Set minimum value.
        void setMinValue(int value);

        //! Get maximum value.
        int maxValue() const;

        //! Set maximum value.
        void setMaxValue(int value);

        //! Set range.
        void setRange(int min, int max);

        //! Get prefix.
        QString prefix() const;

        //! Set prefix.
        void setPrefix(const QString &text);

        //! Get suffix.
        QString suffix() const;

        //! Set suffix.
        void setSuffix(const QString &text);

        //! Get special value text.
        QString specialValueText() const;

        //! Set special value text.
        void setSpecialValueText(const QString &text);

        //! Get value.
        int value() const;

    public slots:
        //! Set value.
        void setValue(int value);

    private:
        QLabel *_label;

        QSlider *_slider;

        QSpinBox *_sbox;

    private slots:
        //! On slider value changed.
        void onSliderValueChanged(int value);

        //! On spinbox value changed.
        void onSpinBoxValueChanged(int value);

};

#endif
