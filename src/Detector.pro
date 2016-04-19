#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T12:45:22
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Detector
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

CONFIG += c++11

RC_ICONS = app.ico

SOURCES += main.cpp\
    recorder.cpp \
    mainwindow.cpp \
    settings.cpp \
    dialog.cpp \
    graphicsscene.cpp \
    actualdetector.cpp \
    videowidget.cpp \
    clickablelabel.cpp \
    camera.cpp \
    messageupdate.cpp \
    imageexplorer.cpp \
    Ctracker.cpp\
    Detector.cpp\
    Kalman.cpp\
    HungarianAlg.cpp\
    BackgroundSubtract.cpp\
    VIBE.cpp \
    uploader.cpp

HEADERS  += \
    recorder.h \
    mainwindow.h \
    settings.h \
    dialog.h \
    graphicsscene.h \
    actualdetector.h \
    videowidget.h \
    clickablelabel.h \
    camera.h \
    messageupdate.h \
    imageexplorer.h \
    uploader.h

FORMS    += \
    mainwindow.ui \
    settings.ui \
    dialog.ui \
    messageupdate.ui \
    imageexplorer.ui \
    uploader.ui

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
