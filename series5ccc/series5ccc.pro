#-------------------------------------------------
#
# Project created by QtCreator 2016-05-19T15:32:57
#
#-------------------------------------------------

QT       -= gui
QT      += widgets sql axcontainer

CONFIG += c++11

TARGET = ../../general/debug/ConnectDrivers/series5ccc
TEMPLATE = lib

INCLUDEPATH    += ../general

DEFINES += SERIES5CCC_LIBRARY

SOURCES += series5ccc.cpp \
    dialog.cpp \
    resultdialog.cpp \
    Opcda_i.c

HEADERS += series5ccc.h\
    ../trei5b/dataineterface.h \
    dialog.h \
    resultdialog.h \
    opcda.h \
    opc.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    series5cccplugin.json

FORMS += \
    dialog.ui \
    resultdialog.ui
