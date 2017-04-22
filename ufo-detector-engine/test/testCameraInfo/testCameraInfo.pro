#-------------------------------------------------
#
# Project created by QtCreator 2016-05-01T15:52:57
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = testcamerainfo
CONFIG += console testcase
CONFIG -= app_bundle

TEMPLATE = app

include(../../opencv.pri)

INCLUDEPATH += ../..

HEADERS += ../../camerainfo.h

SOURCES += testcamerainfo.cpp \
    ../../camerainfo.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_

