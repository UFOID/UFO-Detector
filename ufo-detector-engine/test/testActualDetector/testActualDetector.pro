#-------------------------------------------------
#
# Project created by QtCreator 2016-06-07T11:54:14
#
#-------------------------------------------------

QT       += widgets testlib xml network

TARGET = testActualDetector
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14

CONFIG += c++14

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_

include(../../opencv.pri)

INCLUDEPATH += . \
    ../.. \
    ../mock \
    ../../../ui-qt

SOURCES += \
    ../../actualdetector.cpp \
    ../mock/mockconfig.cpp \
    ../mock/mockcamera.cpp \
    ../mock/mockRecorder.cpp \
    ../mock/mockMainWindow.cpp \
    ../../Ctracker.cpp \
    ../../Detector.cpp \
    ../../Kalman.cpp \
    ../../HungarianAlg.cpp \
    ../../BackgroundSubtract.cpp \
    ../../VIBE.cpp \
    ../mock/mockVideoCodecSupportInfo.cpp \
    testActualDetector.cpp\
    ../../planechecker.cpp \
   ../../detectorstate.cpp


HEADERS += ../../actualdetector.h \
    ../../config.h \
    ../../camera.h \
    ../../recorder.h \
    ../../Ctracker.h \
    ../../Detector.h \
    ../../Kalman.h \
    ../../HungarianAlg.h \
    ../../BackgroundSubtract.h \
    ../../videocodecsupportinfo.h \
    ../../planechecker.h \
    ../../detectorstate.h \
    ../../../ui-qt/mainwindow.h


