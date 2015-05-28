#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include "asliderwidget.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ASliderWidget::ASliderWidget(QWidget *parent) : QWidget(parent) {
    _label = new QLabel(this);

    _slider = new QSlider(this);
    _slider->setOrientation(Qt::Horizontal);
    connect(_slider, SIGNAL(valueChanged(int))
        , this, SLOT(onSliderValueChanged(int)));

    _sbox = new QSpinBox(this);
    connect(_sbox, SIGNAL(valueChanged(int))
        , this, SLOT(onSpinBoxValueChanged(int)));

    QGridLayout *layout = new QGridLayout();
    layout->setMargin(4);
    layout->setSpacing(4);
    layout->addWidget(_label, 0, 0, 1, 2);
    layout->addWidget(_slider, 1, 0, 1, 1);
    layout->addWidget(_sbox, 1, 1, 1, 1);
    layout->setColumnStretch(0,2);
    layout->setColumnStretch(1,1);

    setLayout(layout);
}


// ========================================================================== //
// Get label text.
// ========================================================================== //
QString ASliderWidget::labelText() const {return _label->text();}


// ========================================================================== //
// Set label text.
// ========================================================================== //
void ASliderWidget::setLabelText(const QString &text) {_label->setText(text);}


// ========================================================================== //
// Get minimum value.
// ========================================================================== //
int ASliderWidget::minValue() const {return _slider->minimum();}


// ========================================================================== //
// Set minimum value.
// ========================================================================== //
void ASliderWidget::setMinValue(int value) {
    _slider->setMinimum(value); _sbox->setMinimum(value);
}


// ========================================================================== //
// Get maximum value.
// ========================================================================== //
int ASliderWidget::maxValue() const {return _slider->maximum();}


// ========================================================================== //
// Set maximum value.
// ========================================================================== //
void ASliderWidget::setMaxValue(int value) {
    _slider->setMaximum(value); _sbox->setMaximum(value);
}


// ========================================================================== //
// Set range.
// ========================================================================== //
void ASliderWidget::setRange(int min, int max) {
    _slider->setRange(min, max); _sbox->setRange(min, max);
}


// ========================================================================== //
// Get prefix.
// ========================================================================== //
QString ASliderWidget::prefix() const {return _sbox->prefix();}


// ========================================================================== //
// Set prefix.
// ========================================================================== //
void ASliderWidget::setPrefix(const QString &text) {_sbox->setPrefix(text);}


// ========================================================================== //
// Get suffix.
// ========================================================================== //
QString ASliderWidget::suffix() const {return _sbox->suffix();}


// ========================================================================== //
// Set suffix.
// ========================================================================== //
void ASliderWidget::setSuffix(const QString &text) {_sbox->setSuffix(text);}


// ========================================================================== //
// Get special value text.
// ========================================================================== //
QString ASliderWidget::specialValueText() const {
    return _sbox->specialValueText();
}


// ========================================================================== //
// Set special value text.
// ========================================================================== //
void ASliderWidget::setSpecialValueText(const QString &text) {
    _sbox->setSpecialValueText(text);
}


// ========================================================================== //
// Get value.
// ========================================================================== //
int ASliderWidget::value() const {return _slider->value();}


// ========================================================================== //
// Set value.
// ========================================================================== //
void ASliderWidget::setValue(int value) {
    _slider->setValue(value); _sbox->setValue(value);
}


// ========================================================================== //
// On slider value changed.
// ========================================================================== //
void ASliderWidget::onSliderValueChanged(int value) {
    QSignalBlocker sig_blocker(_sbox);
    _sbox->setValue(value);

    emit valueChanged(value);
}


// ========================================================================== //
// On spinbox value changed.
// ========================================================================== //
void ASliderWidget::onSpinBoxValueChanged(int value) {
    QSignalBlocker sig_blocker(_slider);
    _slider->setValue(value);

    emit valueChanged(value);
}
