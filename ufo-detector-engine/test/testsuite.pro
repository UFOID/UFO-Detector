QMAKE_CXXFLAGS += --coverage
QMAKE_LFLAGS += --coverage


TEMPLATE = subdirs

SUBDIRS = \
    testConfig \
    testCameraInfo \
    testRecorder \
    testActualDetector \
    testVideoCodecSupportInfo



LIBS += -lgcov
