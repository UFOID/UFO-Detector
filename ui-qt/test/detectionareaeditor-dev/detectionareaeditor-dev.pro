#-------------------------------------------------
#
# Project created by QtCreator 2017-04-12T10:34:03
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = detectionareaeditor-dev
TEMPLATE = app

INCLUDEPATH += ../.. \
    ../../../ufo-detector-engine

include(../../../ufo-detector-engine/opencv.pri)

SOURCES += ./main.cpp\
    ./mainwindow.cpp \
    ../../../ufo-detector-engine/config.cpp \
    ../../graphicsscene.cpp \
    ../../detectionareaeditdialog.cpp \
    ../../../ufo-detector-engine/camera.cpp \
    ../../../ufo-detector-engine/camerainfo.cpp \
    ../../../ufo-detector-engine/videocodecsupportinfo.cpp

HEADERS  += ./mainwindow.h \
    ../../../ufo-detector-engine/config.h \
    ../../graphicsscene.h \
    ../../detectionareaeditdialog.h \
    ../../../ufo-detector-engine/camera.h \
    ../../../ufo-detector-engine/camerainfo.h \
    ../../../ufo-detector-engine/videocodecsupportinfo.h

FORMS    += mainwindow.ui \
    ../../detectionareaeditdialog.ui
