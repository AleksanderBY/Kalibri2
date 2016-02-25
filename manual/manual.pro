#-------------------------------------------------
#
# Project created by QtCreator 2016-02-23T16:18:31
#
#-------------------------------------------------

QT       -= gui
QT      += widgets
CONFIG     += plugin
TARGET = ../../general/debug/CPADrivers/manual
TEMPLATE = lib

INCLUDEPATH    += ../general

DEFINES += MANUAL_LIBRARY

SOURCES += manual.cpp \
    setupdialog.cpp

HEADERS += manual.h \
    setupdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    manualplugin.json

FORMS += \
    setupdialog.ui

