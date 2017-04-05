#-------------------------------------------------
#
# Project created by QtCreator 2017-03-31T03:27:34
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = testvideobuffer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../../opencv.pri)

INCLUDEPATH += ../..

SOURCES += testvideobuffer.cpp \
    ../../videobuffer.cpp
HEADERS += ../../videobuffer.h

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_
