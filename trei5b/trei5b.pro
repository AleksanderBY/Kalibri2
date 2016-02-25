#-------------------------------------------------
#
# Project created by QtCreator 2015-11-20T16:23:10
#
#-------------------------------------------------

QT       -= gui
QT      += widgets sql

TARGET = ../../general/debug/ConnectDrivers/trei5b
TEMPLATE = lib
INCLUDEPATH    += ../general

DEFINES += TREI5B_LIBRARY

SOURCES += trei.cpp \
    dialog.cpp

HEADERS += trei.h \
    dataineterface.h \
    CLIE0DEF.H \
    dialog.h



unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    treiplugin.json




unix:!macx|win32: LIBS += -L$$PWD/ -lIsaClientLib

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/IsaClientLib.lib

unix:!macx|win32: LIBS += -L$$PWD/../../../../Tools/mingw491_32/i686-w64-mingw32/lib/ -lws2_32

INCLUDEPATH += $$PWD/../../../../Tools/mingw491_32/i686-w64-mingw32/include
DEPENDPATH += $$PWD/../../../../Tools/mingw491_32/i686-w64-mingw32/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../../Tools/mingw491_32/i686-w64-mingw32/lib/ws2_32.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/../../../../Tools/mingw491_32/i686-w64-mingw32/lib/libws2_32.a

FORMS += \
    dialog.ui
