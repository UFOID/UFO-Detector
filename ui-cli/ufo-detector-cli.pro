QT += core
QT -= gui

unix {
    QMAKE_CXXFLAGS += -std=c++1y
    CONFIG += c++1y
}

TARGET = ufo-detector-cli
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

include(../ufo-detector-engine/ufo-detector-engine.pri)

SOURCES += main.cpp \
    console.cpp

HEADERS += \
    console.h

