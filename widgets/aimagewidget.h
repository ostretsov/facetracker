#ifndef AIMAGEWIDGET_H
#define AIMAGEWIDGET_H

#include <QtGui/QImage>

#include <QtWidgets/QWidget>

class AImageWidget : public QWidget {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AImageWidget(QWidget *parent = NULL);

        //! Destructor.
        virtual ~AImageWidget() {}

        //! Get image.
        QImage image() const;

        //! Get image size.
        QSize imageSize() const;

        //! Get destination rect.
        QRect destRect() const;

        //! Get size hint.
        virtual QSize sizeHint() const;

    public slots:
        //! Update image.
        void updateImage(const QImage &image);

        //! Update roi.
        void updateRoi(const QRect &roi);

    protected:
        //! Paint event.
        virtual void paintEvent(QPaintEvent *event);

        //! Resize event.
        virtual void resizeEvent(QResizeEvent *event);

    private:
        QImage _image;

        QRect _dst_rc, _roi;

};

#endif
