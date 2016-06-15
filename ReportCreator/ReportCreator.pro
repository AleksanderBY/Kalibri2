#-------------------------------------------------
#
# Project created by QtCreator 2016-06-10T08:26:20
#
#-------------------------------------------------

QT       -= core
QT       += widgets

TARGET = ../../general/debug/ReportCreator
TEMPLATE = lib

DEFINES += REPORTCREATOR_LIBRARY

SOURCES += reportcreator.cpp \
    mainwindow.cpp \
    qtexteditcompleter.cpp

HEADERS += reportcreator.h \
    reportcreatorinterface.h \
    mainwindow.h \
    qtexteditcompleter.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    reportcreatorplugin.json

FORMS += \
    mainwindow.ui
