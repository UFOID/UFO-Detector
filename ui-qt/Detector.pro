#-------------------------------------------------
#
# Project created by QtCreator 2016-12-04T12:45:22
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ufodetector
TEMPLATE = app

win32:CONFIG += windows
win64:CONFIG += windows

unix {
    QMAKE_CXXFLAGS += -std=c++1y
    CONFIG += c++1y
}

RC_ICONS = app.ico

VPATH += ../ufo-detector-engine
INCLUDEPATH += ../ufo-detector-engine
include(../ufo-detector-engine/ufo-detector-engine.pri)

SOURCES += main.cpp\
    mainwindow.cpp \
    graphicsscene.cpp \
    videowidget.cpp \
    clickablelabel.cpp \
    imageexplorer.cpp \
    settingsdialog.cpp \
    detectionareaeditdialog.cpp \
    cameraresolutiondialog.cpp \
    videolist.cpp \
    videouploaderdialog.cpp \
    updateapplicationdialog.cpp

HEADERS  += \
    mainwindow.h \
    graphicsscene.h \
    videowidget.h \
    clickablelabel.h \
    imageexplorer.h \
    settingsdialog.h \
    detectionareaeditdialog.h \
    cameraresolutiondialog.h \
    videolist.h \
    videouploaderdialog.h \
    updateapplicationdialog.h

FORMS    += \
    mainwindow.ui \
    imageexplorer.ui \
    settingsdialog.ui \
    detectionareaeditdialog.ui \
    cameraresolutiondialog.ui \
    videouploaderdialog.ui \
    updateapplicationdialog.ui

RESOURCES = detector.qrc

DISTFILES += \
    detector.qrc \
    detector.qss

TRANSLATIONS += ../lang/detector_fi.ts
