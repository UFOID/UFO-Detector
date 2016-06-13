#-------------------------------------------------
#
# Project created by QtCreator 2016-05-01T09:28:12
#
#-------------------------------------------------

QT       += testlib core widgets

#QT       -= gui

TARGET = testconfig
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += _UNIT_TEST_

include(../../src/opencv.pri)

INCLUDEPATH += ../../src \
    ../mock

SOURCES += testconfig.cpp \
    ../../src/config.cpp \
    ../mock/mockVideoCodecSupportInfo.cpp

HEADERS += ../../src/config.h \
    ../../src/videocodecsupportinfo.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"
