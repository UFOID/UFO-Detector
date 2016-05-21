
#include "recorder.h"
#include "actualdetector.h"
#include "camera.h"
#include "config.h"
#include "mainwindow.h"
#include <QString>
#include <QtTest>

class TestRecorder : public QObject
{
    Q_OBJECT

public:
    TestRecorder();
    ~TestRecorder();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testConstructor();

private:
    Recorder* m_recorder;
    MainWindow* m_mainWindow;
    Config* m_config;
    ActualDetector* m_actualDetector;
    Camera* m_camera;
};

TestRecorder::TestRecorder() {
    m_config = new Config();
    m_camera = new Camera(0, m_config->cameraWidth(), m_config->cameraHeight());
    m_mainWindow = new MainWindow(NULL, m_camera, m_config);
    m_actualDetector = new ActualDetector(m_mainWindow, m_camera, m_config);
    m_recorder = new Recorder(m_actualDetector, m_camera, m_config);
}

TestRecorder::~TestRecorder() {
    m_recorder->deleteLater();
    m_actualDetector->deleteLater();
    m_mainWindow->deleteLater();
    m_camera->deleteLater();
    m_config->deleteLater();
}

void TestRecorder::initTestCase() {

}

void TestRecorder::cleanupTestCase() {
}

void TestRecorder::testConstructor() {
    QVERIFY(NULL != m_recorder);
}

QTEST_MAIN(TestRecorder)

#include "testrecorder.moc"
