#ifndef ASERVICEMETATYPECONTROLLER_H
#define ASERVICEMETATYPECONTROLLER_H

#include <QtCore/QObject>

class AServiceMetatypeController : public QObject {
    Q_OBJECT

    public:
        //! Register meta types.
        static void registerMetaTypes();

        //! Constructor.
        explicit AServiceMetatypeController(QObject *parent = NULL);

        //! Destructor.
        virtual ~AServiceMetatypeController() {}

};

#endif
