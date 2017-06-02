QT += core
QT -= gui

unix {
    # NOTE: c++14 is not supported in Raspberry Pi on Qemu
    QMAKE_CXXFLAGS += -std=c++11    # 14: c++1y
    CONFIG += c++11
}

TARGET = ufo-detector-cli
CONFIG += console
CONFIG -= app_bundle

# enable this to compile for Raspberry Pi etc.
#CONFIG += embedded

TEMPLATE = app

include(../ufo-detector-engine/ufo-detector-engine.pri)

SOURCES += main.cpp \
    console.cpp

HEADERS += \
    console.h

embedded {
    INSTALLS += target
    target.files = ufo-detector-cli
    target.path = /home/pi/bin
}
