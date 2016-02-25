#-------------------------------------------------
#
# Project created by QtCreator 2015-11-17T13:27:49
#
#-------------------------------------------------

QT       -= gui
QT             += widgets serialport
CONFIG     += plugin
TARGET = ../../general/debug/CPADrivers/iksu2000
TEMPLATE = lib

INCLUDEPATH    += ../general

SOURCES += iksu2000.cpp \
    dialog.cpp \
    setupdialog.cpp

HEADERS += iksu2000.h \
    dialog.h \
    setupdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    iksu2000plugin.json \
    ../foto_iksu_2000.jpg

FORMS += \
    dialog.ui \
    setupdialog.ui


