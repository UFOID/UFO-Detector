#-------------------------------------------------
#
# Project created by QtCreator 2016-06-12T20:42:53
#
#-------------------------------------------------

QT       += testlib core widgets

QT       -= gui

TARGET = testVideoCodecSupportInfo
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

CONFIG += c++11

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_

include(../../src/opencv.pri)

INCLUDEPATH += ../../src \
    ../mock

HEADERS += ../../src/videocodecsupportinfo.h \
    ../../src/config.h

SOURCES += testVideoCodecSupportInfo.cpp \
    ../../src/videocodecsupportinfo.cpp \
    ../mock/mockconfig.cpp

