#-------------------------------------------------
#
# Project created by QtCreator 2015-11-17T13:21:26
#
#-------------------------------------------------

QT       += core gui
QT += widgets xml sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = general
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    kaliblri2setting.cpp \
    kalibri2.cpp \
    settingsdialog.cpp \
    createdialog.cpp \
    calibrator.cpp \
    aloggermodel.cpp \
    axmlcalibrationmodel.cpp \
    environmentdialog.cpp \
    aenvironment.cpp

HEADERS  += mainwindow.h \
    cpainterface.h \
    kaliblri2setting.h \
    kalibri2.h \
    settingsdialog.h \
    createdialog.h \
    ../trei5b/dataineterface.h \
    calibrator.h \
    aloggermodel.h \
    axmlcalibrationmodel.h \
    environmentdialog.h \
    aenvironment.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    createdialog.ui \
    environmentdialog.ui

DISTFILES += \
    settings.xml

INSTALLS += target
