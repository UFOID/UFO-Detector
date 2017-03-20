
TEMPLATE = subdirs

SUBDIRS = \
    testConfig \
    testCameraInfo \
    testRecorder \
    testActualDetector \
    testVideoCodecSupportInfo

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0

LIBS += \
-lgcov
