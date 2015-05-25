QT += core gui widgets sql network

TARGET = facetracker

TEMPLATE = app

TRANSLATIONS = facetracker_ru.ts

SOURCES += main.cpp \
    helpers/asettingshelper.cpp \
    aservicecontroller.cpp \
    aservicemetatypecontroller.cpp \
    systemtrayicon/asystemtrayicon.cpp \
    systemtrayicon/auniformsystemtrayicon.cpp \
    acapturethread.cpp \
    widgets/aimagewidget.cpp \
    database/adatabasecontroller.cpp \
    database/asqltablecontroller.cpp \
    database/atablecontroller.cpp \
    amessagessqltablecontroller.cpp \
    aservicedatabasecontroller.cpp \
    asettingsdialog.cpp \
    astatisticsqltablecontroller.cpp \
    arsssqltablecontroller.cpp \
    afacecontroller.cpp \
    afacecapturethread.cpp

HEADERS += \
    helpers/asettingshelper.h \
    aservicecontroller.h \
    aservicemetatypecontroller.h \
    systemtrayicon/asystemtrayicon.h \
    systemtrayicon/auniformsystemtrayicon.h \
    acapturethread.h \
    widgets/aimagewidget.h \
    database/adatabasecontroller.h \
    database/asqltablecontroller.h \
    database/atablecontroller.h \
    amessagessqltablecontroller.h \
    aservicedatabasecontroller.h \
    asettingsdialog.h \
    astatisticsqltablecontroller.h \
    arsssqltablecontroller.h \
    afacecontroller.h \
    afacecapturethread.h

unix {
    INCLUDEPATH += /usr/include/libappindicator-0.1 \
        /usr/include/gtk-2.0 \
        /usr/lib/gtk-2.0/include \
        /usr/include/glib-2.0 \
        /usr/lib/glib-2.0/include \
        /usr/include/cairo \
        /usr/include/atk-1.0 \
        /usr/include/pango-1.0

    SOURCES += systemtrayicon/aunitysystemtrayicon.cpp

    HEADERS += systemtrayicon/aunitysystemtrayicon.h

    LIBS += -L/usr/lib -lappindicator -lnotify

    CONFIG += link_pkgconfig

    PKGCONFIG += gtk+-2.0 opencv
}

win32 {
    INCLUDEPATH += $$PWD/../opencv/build/x86/mingw4.9.1/install/include

    LIBS += -L$$PWD/../opencv/build/x86/mingw4.9.1/bin/ \
        -lopencv_core300 \
        -lopencv_imgproc300 \
        -lopencv_imgcodecs300 \
        -lopencv_objdetect300 \
        -lopencv_videoio300 \
        -lopencv_highgui300
}

RESOURCES += \
    classifiers.qrc \
    images.qrc
