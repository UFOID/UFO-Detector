#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T12:45:22
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ufodetector
TEMPLATE = app

win32:CONFIG += windows
win64:CONFIG += windows

QMAKE_CXXFLAGS += -std=c++11

CONFIG += c++11

RC_ICONS = app.ico

SOURCES += main.cpp\
    recorder.cpp \
    mainwindow.cpp \
    graphicsscene.cpp \
    actualdetector.cpp \
    videowidget.cpp \
    clickablelabel.cpp \
    camera.cpp \
    imageexplorer.cpp \
    Ctracker.cpp\
    Detector.cpp\
    Kalman.cpp\
    HungarianAlg.cpp\
    BackgroundSubtract.cpp\
    VIBE.cpp \
    settingsdialog.cpp \
    config.cpp \
    detectionareaeditdialog.cpp \
    camerainfo.cpp \
    cameraresolutiondialog.cpp \
    videolist.cpp \
    videouploaderdialog.cpp \
    updateapplicationdialog.cpp

HEADERS  += \
    recorder.h \
    mainwindow.h \
    graphicsscene.h \
    actualdetector.h \
    videowidget.h \
    clickablelabel.h \
    camera.h \
    imageexplorer.h \
    Ctracker.h \
    Detector.h \
    Kalman.h \
    HungarianAlg.h \
    BackgroundSubtract.h \
    VIBE.h \
    OpencvInclude.h \
    settingsdialog.h \
    config.h \
    detectionareaeditdialog.h \
    camerainfo.h \
    cameraresolutiondialog.h \
    videolist.h \
    videouploaderdialog.h \
    updateapplicationdialog.h

FORMS    += \
    mainwindow.ui \
    imageexplorer.ui \
    settingsdialog.ui \
    detectionareaeditdialog.ui \
    cameraresolutiondialog.ui \
    videouploaderdialog.ui \
    updateapplicationdialog.ui

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

RESOURCES = detector.qrc

DISTFILES += \
    detector.qrc \
    detector.qss

TRANSLATIONS += ../lang/detector_fi.ts
