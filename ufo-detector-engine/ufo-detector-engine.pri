
QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:CONFIG += windows
win64:CONFIG += windows


include(opencv.pri)

# https://bugreports.qt.io/browse/QTBUG-4329
INCLUDEPATH += $$PWD

SOURCES += $$PWD/recorder.cpp \

    $$PWD/actualdetector.cpp \
    $$PWD/camera.cpp \
    $$PWD/Ctracker.cpp \
    $$PWD/Detector.cpp \
    $$PWD/Kalman.cpp \
    $$PWD/HungarianAlg.cpp \
    $$PWD/config.cpp \
    $$PWD/camerainfo.cpp \
    $$PWD/videobuffer.cpp \
    $$PWD/videocodecsupportinfo.cpp \
    $$PWD/planechecker.cpp \
    $$PWD/detectorstate.cpp

HEADERS  += $$PWD/defines.h \
    $$PWD/recorder.h \
    $$PWD/actualdetector.h \
    $$PWD/camera.h \
    $$PWD/Ctracker.h \
    $$PWD/Detector.h \
    $$PWD/Kalman.h \
    $$PWD/HungarianAlg.h \
    $$PWD/config.h \
    $$PWD/camerainfo.h \
    $$PWD/videobuffer.h \
    $$PWD/videocodecsupportinfo.h \
    $$PWD/OpencvInclude.h \
    $$PWD/planechecker.h \
    $$PWD/detectorstate.h