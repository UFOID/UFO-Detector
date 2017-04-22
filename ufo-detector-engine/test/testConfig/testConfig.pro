#-------------------------------------------------
#
# Project created by QtCreator 2016-05-01T09:28:12
#
#-------------------------------------------------

QT       += testlib core widgets

#QT       -= gui

TARGET = testconfig
CONFIG += console testcase
CONFIG -= app_bundle

TEMPLATE = app

DEFINES += _UNIT_TEST_

INCLUDEPATH += ../.. \
    ../mock

include(../../opencv.pri)

SOURCES += testconfig.cpp \
    ../../config.cpp \
    ../mock/mockVideoCodecSupportInfo.cpp

HEADERS += ../../config.h \
    ../../videocodecsupportinfo.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"
