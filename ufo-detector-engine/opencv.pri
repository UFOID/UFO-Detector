
win32:CONFIG += windows
win64:CONFIG += windows

windows {
    INCLUDEPATH += C:\own\install\include
    LIBS += -LC:\own\install\x64\vc14\lib
    LIBS += -lopencv_core320
    LIBS += -lopencv_highgui320
    LIBS += -lopencv_imgproc320
    LIBS += -lopencv_calib3d320
    LIBS += -lopencv_features2d320
    LIBS += -lopencv_flann320
    LIBS += -lopencv_ml320
    LIBS += -lopencv_objdetect320
    LIBS += -lopencv_video320
    LIBS += -lopencv_videoio320
    LIBS += -lopencv_imgcodecs320

}



unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}
