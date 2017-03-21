QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage


TEMPLATE = subdirs

SUBDIRS = \
    testConfig \
    testCameraInfo \
    testRecorder \
    testActualDetector \
    testVideoCodecSupportInfo



LIBS += \
-lgcov
