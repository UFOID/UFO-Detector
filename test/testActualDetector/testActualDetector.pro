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

QMAKE_CXXFLAGS += -std=c++11

CONFIG += c++11

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_

include(../../src/opencv.pri)

INCLUDEPATH += . \
    ../../src \
    ../mock

SOURCES += \
    ../../src/actualdetector.cpp \
    ../mock/mockconfig.cpp \
    ../mock/mockcamera.cpp \
    ../mock/mockMainWindow.cpp \
    ../mock/mockRecorder.cpp \
    ../../src/Ctracker.cpp \
    ../../src/Detector.cpp \
    ../../src/Kalman.cpp \
    ../../src/HungarianAlg.cpp \
    ../../src/BackgroundSubtract.cpp \
    ../../src/VIBE.cpp \
    ../mock/mockVideoCodecSupportInfo.cpp \
    testActualDetector.cpp

HEADERS += ../../src/actualdetector.h \
    ../../src/config.h \
    ../../src/camera.h \
    ../../src/recorder.h \
    ../../src/mainwindow.h \
    ../../src/Ctracker.h \
    ../../src/Detector.h \
    ../../src/Kalman.h \
    ../../src/HungarianAlg.h \
    ../../src/BackgroundSubtract.h \
    ../../src/VIBE.h \
    ../../src/videocodecsupportinfo.h


