
win32:CONFIG += windows
win64:CONFIG += windows

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
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}
