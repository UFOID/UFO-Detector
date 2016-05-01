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

INCLUDEPATH += ../../src

SOURCES += testconfig.cpp \
    ../../src/config.cpp
HEADERS += ../../src/config.h
DEFINES += SRCDIR=\\\"$$PWD/\\\"
