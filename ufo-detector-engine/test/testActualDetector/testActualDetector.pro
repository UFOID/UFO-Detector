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

QMAKE_CXXFLAGS += -std=c++14 --coverage
QMAKE_LFLAGS += --coverage

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
    ../../Ctracker.cpp \
    ../../Detector.cpp \
    ../../Kalman.cpp \
    ../../HungarianAlg.cpp \
    ../mock/mockVideoCodecSupportInfo.cpp \
    testActualDetector.cpp\
    ../../planechecker.cpp \
   ../../detectorstate.cpp \
    ../mock/mockdatamanager.cpp


HEADERS += ../../actualdetector.h \
    ../../config.h \
    ../../camera.h \
    ../../recorder.h \
    ../../Ctracker.h \
    ../../Detector.h \
    ../../Kalman.h \
    ../../HungarianAlg.h \
    ../../videocodecsupportinfo.h \
    ../../planechecker.h \
    ../../detectorstate.h \
    ../../datamanager.h


