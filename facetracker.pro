QT += core gui widgets sql network xml

TARGET = facetracker

TEMPLATE = app

CONFIG += c++11

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
    afacecapturethread.cpp \
    asessioncontroller.cpp \
    requests/arequest.cpp \
    requests/aftcomrequest.cpp \
    requests/aloginftcomrequest.cpp \
    requests/alogoutftcomrequest.cpp \
    requests/asyncftcomrequest.cpp \
    requests/aversionftcomrequest.cpp \
    requests/arssftcomrequest.cpp \
    widgets/asliderwidget.cpp

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
    afacecapturethread.h \
    asessioncontroller.h \
    requests/arequest.h \
    requests/aftcomrequest.h \
    requests/aloginftcomrequest.h \
    requests/alogoutftcomrequest.h \
    requests/asyncftcomrequest.h \
    requests/aversionftcomrequest.h \
    requests/arssftcomrequest.h \
    widgets/asliderwidget.h

DEFINES += __STDC_CONSTANT_MACROS

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

    PKGCONFIG += gtk+-2.0 opencv \
        libavdevice libavutil libavformat libavcodec libswscale
}

win32 {
    INCLUDEPATH += $$PWD/../opencv/build/x86/mingw4.9.1/install/include \
        $$PWD/../ffmpeg/include

    LIBS += -L$$PWD/../opencv/build/x86/mingw4.9.1/bin/ \
        -L$$PWD/../ffmpeg/bin/ \
        -lopencv_core300 \
        -lopencv_imgproc300 \
        -lopencv_imgcodecs300 \
        -lopencv_objdetect300 \
        -lavdevice-56 \
        -lavformat-56 \
        -lavutil-54 \
        -lavcodec-56 \
        -lswscale-3
}

RESOURCES += \
    classifiers.qrc \
    images.qrc

DISTFILES += \
    facetracker_ru.ts
