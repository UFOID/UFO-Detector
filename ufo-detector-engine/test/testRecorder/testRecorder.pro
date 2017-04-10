#-------------------------------------------------
#
# Project created by QtCreator 2016-05-21T12:16:19
#
#-------------------------------------------------

QT       += testlib core xml widgets network

TARGET = testrecorder
CONFIG += console testcase
CONFIG -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

CONFIG += c++11

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_

include(../../opencv.pri)

INCLUDEPATH += . \
    ../.. \
    ../mock

SOURCES += testrecorder.cpp \
    ../mock/mockcamera.cpp \
    ../mock/mockconfig.cpp \
    ../mock/mockdatamanager.cpp \
    ../mock/mockvideobuffer.cpp \
    ../../videocodecsupportinfo.cpp \
    ../../recorder.cpp \
    ../../camerainfo.cpp

HEADERS += ../../recorder.h \
    ../../config.h \
    ../../videocodecsupportinfo.h \
    ../../camera.h \
    ../../camerainfo.h \
    ../../datamanager.h \
    ../../videobuffer.h

