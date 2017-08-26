
win32:CONFIG += windows
win64:CONFIG += windows

windows {
    INCLUDEPATH += C:\OpenCV\build\install\include
    LIBS += -LC:\OpenCV\build\install\x64\vc15\lib
    LIBS += -lopencv_core330
    LIBS += -lopencv_highgui330
    LIBS += -lopencv_imgproc330
    LIBS += -lopencv_calib3d330
    LIBS += -lopencv_features2d330
    LIBS += -lopencv_flann330
    LIBS += -lopencv_ml330
    LIBS += -lopencv_objdetect330
    LIBS += -lopencv_video330
    LIBS += -lopencv_videoio330
    LIBS += -lopencv_imgcodecs330

}


unix {
    embedded {
        message("Building for ARM / Raspberry Pi / etc.")
        LIBS += -L$$[QT_SYSROOT]/usr/lib/arm-linux-gnueabihf
        LIBS += -L/usr/lib/arm-linux-gnueabihf
        INCLUDEPATH += $$[QT_SYSROOT]/usr/lib/arm-linux-gnueabihf
        INCLUDEPATH += /usr/lib/arm-linux-gnueabihf
        INCLUDEPATH += /usr/include/opencv
        LIBS += -lopencv_calib3d
        LIBS += -lopencv_contrib
        LIBS += -lopencv_core
        LIBS += -lopencv_features2d
        LIBS += -lopencv_flann
        LIBS += -lopencv_highgui
        LIBS += -lopencv_imgproc
        LIBS += -lopencv_ml
        LIBS += -lopencv_objdetect
        LIBS += -lopencv_video
    } else {
        CONFIG += link_pkgconfig
        PKGCONFIG += opencv
    }
}
