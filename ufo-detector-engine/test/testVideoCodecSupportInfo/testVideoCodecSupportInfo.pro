#-------------------------------------------------
#
# Project created by QtCreator 2016-06-12T20:42:53
#
#-------------------------------------------------

QT       += testlib core widgets

QT       -= gui

TARGET = testVideoCodecSupportInfo
CONFIG += console testcase
CONFIG -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

CONFIG += c++11

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_

include(../../opencv.pri)

INCLUDEPATH += ../..

HEADERS += ../../videocodecsupportinfo.h

SOURCES += testVideoCodecSupportInfo.cpp \
    ../../videocodecsupportinfo.cpp

