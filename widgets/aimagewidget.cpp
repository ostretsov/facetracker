#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include "aimagewidget.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AImageWidget::AImageWidget(QWidget *parent) : QWidget(parent) {
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Qt::gray);
    palette.setColor(QPalette::Background, Qt::gray);
    setPalette(palette);
}


// ========================================================================== //
// Get image.
// ========================================================================== //
QImage AImageWidget::image() const {return _image;}


// ========================================================================== //
// Get image size.
// ========================================================================== //
QSize AImageWidget::imageSize() const {return _image.size();}


// ========================================================================== //
// Get destination rect.
// ========================================================================== //
QRect AImageWidget::destRect() const {return _dst_rc;}


// ========================================================================== //
// Get size hint.
// ========================================================================== //
QSize AImageWidget::sizeHint() const {return QSize(320,240);}


// ========================================================================== //
// Update image.
// ========================================================================== //
void AImageWidget::updateImage(const QImage &image) {
    if(image.isNull()) _image = QImage();
    else _image = image.copy();

    _roi = QRect();

    update();
}


// ========================================================================== //
// Update roi.
// ========================================================================== //
void AImageWidget::updateRoi(const QRect &roi) {_roi = roi;}


// ========================================================================== //
// Paint event.
// ========================================================================== //
void AImageWidget::paintEvent(QPaintEvent *event) {
    if(_image.isNull()) {
        _dst_rc = QRect();

        QPainter painter(this);
        painter.fillRect(event->rect(), palette().background());
        event->accept();

        return;
    }

    if(_dst_rc.isNull()) {
        QSize img_size = _image.size();
        QSize rc_size  = rect().size();

        if(img_size.width() > rc_size.width()
            || img_size.height() > rc_size.height())
            img_size.scale(rc_size, Qt::KeepAspectRatio);

        QRect dst_rc(QPoint(0,0), img_size);
        dst_rc.moveCenter(rect().center());

        _dst_rc = dst_rc;
    }

    QPainter painter(this);

    if(!_dst_rc.contains(event->rect())) {
        QRegion region = event->region();
        region = region.subtracted(_dst_rc);

        const QBrush &brush = palette().background();
        foreach(const QRect &rc, region.rects()) painter.fillRect(rc, brush);
    }

    painter.drawImage(_dst_rc, _image, _image.rect());

    if(!_roi.isNull()) {
        QRect rect = _roi;

        QTransform trans_scale
            = QTransform::fromScale(
                (qreal)_dst_rc.width()/(qreal)_image.width(),
                (qreal)_dst_rc.height()/(qreal)_image.height());

        rect = trans_scale.mapRect(rect);

        QTransform trans_translate
            = QTransform::fromTranslate((this->width()-_dst_rc.width())/2
                , (this->height()-_dst_rc.height())/2);

        rect = trans_translate.mapRect(rect);

        QPen pen;
        pen.setCosmetic(true);
        pen.setColor(Qt::green);

        painter.save();
        painter.setPen(pen);
        painter.drawRect(rect);
        painter.restore();
    }

    event->accept();
}


// ========================================================================== //
// Resize event.
// ========================================================================== //
void AImageWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event); _dst_rc = QRect();
}
