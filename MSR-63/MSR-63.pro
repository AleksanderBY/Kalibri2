#-------------------------------------------------
#
# Project created by QtCreator 2016-03-23T16:32:26
#
#-------------------------------------------------

QT       -= gui
QT      += widgets
CONFIG     += plugin
TARGET = ../../general/debug/CPADrivers/msr-63
TEMPLATE = lib

INCLUDEPATH    += ../general

DEFINES += MSR63_LIBRARY

SOURCES += msr63.cpp \
    setupdialog.cpp

HEADERS += msr63.h \
    setupdialog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    msr63plugin.json

FORMS += \
    setupdialog.ui
