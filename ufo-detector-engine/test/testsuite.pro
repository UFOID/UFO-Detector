
TEMPLATE = subdirs

SUBDIRS = \
    testConfig \
    testCameraInfo \
    testRecorder \
    testActualDetector \
    testVideoCodecSupportInfo

QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage -fPIC -O0
QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage -fPIC -O0

LIBS += \
-lgcov
