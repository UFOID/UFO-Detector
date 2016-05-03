#-------------------------------------------------
#
# Project created by QtCreator 2016-05-01T15:52:57
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = testcamerainfo
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../src

HEADERS += ../../src/camerainfo.h

SOURCES += testcamerainfo.cpp \
    ../../src/camerainfo.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
    _UNIT_TEST_

windows {
    INCLUDEPATH += C:\own\install\include
    LIBS += C:\own\bin\libopencv_core300.dll
    LIBS += C:\own\bin\libopencv_highgui300.dll
    LIBS += C:\own\bin\libopencv_imgproc300.dll
    LIBS += C:\own\bin\libopencv_calib3d300.dll
    LIBS += C:\own\bin\libopencv_features2d300.dll
    LIBS += C:\own\bin\libopencv_flann300.dll
    LIBS += C:\own\bin\libopencv_ml300.dll
    LIBS += C:\own\bin\libopencv_objdetect300.dll
    LIBS += C:\own\bin\libopencv_video300.dll
    LIBS += C:\own\bin\libopencv_videoio300.dll
    LIBS += C:\own\bin\libopencv_imgcodecs300.dll
}

unix {
    INCLUDEPATH += /usr/include/opencv2/
    LIBS += -lopencv_core
    LIBS += -lopencv_highgui
    LIBS += -lopencv_imgproc
    LIBS += -lopencv_calib3d
    LIBS += -lopencv_features2d
    LIBS += -lopencv_flann
    LIBS += -lopencv_ml
    LIBS += -lopencv_objdetect
    LIBS += -lopencv_video
    #LIBS += -lopencv_videoio
    #LIBS += -lopencv_imgcodecs
}
