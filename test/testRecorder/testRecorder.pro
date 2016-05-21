#-------------------------------------------------
#
# Project created by QtCreator 2016-05-21T12:16:19
#
#-------------------------------------------------

QT       += testlib core xml widgets network

TARGET = testrecorder
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -L. -Xlinker --wrap -Xlinker cv

CONFIG += c++11

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_

include(../../src/opencv.pri)

INCLUDEPATH += . \
    ../../src

SOURCES += testrecorder.cpp \
    mockcamera.cpp \
    mockactualdetector.cpp \
    mockconfig.cpp \
    ../../src/recorder.cpp \
    ../../src/camerainfo.cpp \
    ../../src/Detector.cpp \
    ../../src/videowidget.cpp \
    ../../src/clickablelabel.cpp \
    ../../src/graphicsscene.cpp \
    ../../src/videolist.cpp \
    ../../src/mainwindow.cpp \
    ../../src/BackgroundSubtract.cpp \
    ../../src/Ctracker.cpp \
    ../../src/VIBE.cpp \
    ../../src/Kalman.cpp \
    ../../src/HungarianAlg.cpp \
    ../../src/videouploaderdialog.cpp \
    ../../src/settingsdialog.cpp \
    ../../src/imageexplorer.cpp \
    ../../src/updateapplicationdialog.cpp \
    ../../src/cameraresolutiondialog.cpp \
    ../../src/detectionareaeditdialog.cpp

HEADERS += ../../src/recorder.h \
    ../../src/config.h \
    ../../src/camera.h \
    ../../src/camerainfo.h \
    ../../src/Detector.h \
    ../../src/actualdetector.h \
    ../../src/videowidget.h \
    ../../src/clickablelabel.h \
    ../../src/graphicsscene.h \
    ../../src/videolist.h \
    ../../src/mainwindow.h \
    ../../src/BackgroundSubtract.h \
    ../../src/Ctracker.h \
    ../../src/VIBE.h \
    ../../src/Kalman.h \
    ../../src/HungarianAlg.h \
    ../../src/videouploaderdialog.h \
    ../../src/settingsdialog.h \
    ../../src/imageexplorer.h \
    ../../src/updateapplicationdialog.h \
    ../../src/cameraresolutiondialog.h \
    ../../src/detectionareaeditdialog.h

FORMS += ../../src/mainwindow.ui \
    ../../src/videouploaderdialog.ui \
    ../../src/settingsdialog.ui \
    ../../src/imageexplorer.ui \
    ../../src/updateapplicationdialog.ui \
    ../../src/cameraresolutiondialog.ui \
    ../../src/detectionareaeditdialog.ui
