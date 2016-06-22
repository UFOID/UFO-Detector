#-------------------------------------------------
#
# Project created by QtCreator 2016-05-21T12:16:19
#
#-------------------------------------------------

QT       += testlib core xml widgets

TARGET = testrecorder
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

SOURCES += testrecorder.cpp \
    ../mock/mockcamera.cpp \
    ../mock/mockconfig.cpp \
    ../../src/videocodecsupportinfo.cpp \
    ../../src/recorder.cpp \
    ../../src/camerainfo.cpp

HEADERS += ../../src/recorder.h \
    ../../src/config.h \
    ../../src/videocodecsupportinfo.h \
    ../../src/camera.h \
    ../../src/camerainfo.h

