#-------------------------------------------------
#
# Project created by QtCreator 2017-04-09T20:29:29
#
#-------------------------------------------------

QT       += network testlib xml widgets

QT       -= gui

TARGET = testdatamanager
CONFIG += console testcase
CONFIG -= app_bundle

TEMPLATE = app

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_

INCLUDEPATH += ../../ \
    ../mock

SOURCES += testdatamanager.cpp \
    ../../datamanager.cpp \
    ../mock/mockconfig.cpp \
    ../mock/mockVideoCodecSupportInfo.cpp

HEADERS += ../../datamanager.h \
    ../../config.h \
    ../../videocodecsupportinfo.h
