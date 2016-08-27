/*
 * UFO Detector | www.UFOID.net
 *
 * Copyright (C) 2016 UFOID
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "actualdetector.h"
#include "mainwindow.h"
#include "config.h"
#include "camera.h"
#include <QString>
#include <QtTest>
#include <QCoreApplication>

extern cv::Mat mockCameraNextFrame;
extern void mockCamera_setFrameBlockingEnabled(bool);
extern void mockCamera_releaseNextFrame();

extern int mockRecorderStartCount;
extern int mockRecorderStopCount;

class TestActualDetector : public QObject
{
    Q_OBJECT

public:
    TestActualDetector();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void constructor();

    /**
     * Test blocking Camera::getWebcamFrame(). See a basic non-blocking test in Recorder unit test.
     */
    void mockCameraBlockNextFrame();

    void detectBrightObjects();
    void detectDarkObjects();

private:
    ActualDetector* m_actualDetector;
    MainWindow* m_mainWindow;
    Config* m_config;
    Camera* m_camera;

    // test thread to consume camera frames
    std::thread* m_cameraFrameConsumerThread;
    void cameraFrameConsumerThreadFunc();
    int m_cameraFramesConsumed; ///< number of camera frames received by cameraFrameConsumerThreadFunc
    bool m_consumeCameraFrames; ///< camera frame consumer thread run enabled flag
    bool m_cameraFrameConsumerRunning;  ///< camera frame consumer thread is running

    int m_cameraFps;    ///< frames per second for mock camera

    void makeDetectionAreaFile();

private slots:
    void onActualDetectorStartProgressChanged(int progress);
};

TestActualDetector::TestActualDetector()
{
}

void TestActualDetector::initTestCase()
{
    m_config = new Config();
    QVERIFY(NULL != m_config);
    m_camera = new Camera(m_config->cameraIndex(), m_config->cameraWidth(), m_config->cameraHeight());
    QVERIFY(NULL != m_camera);
    m_mainWindow = new MainWindow(NULL, m_camera, m_config);
    QVERIFY(NULL != m_mainWindow);
    m_actualDetector = new ActualDetector(m_mainWindow, m_camera, m_config);
    QVERIFY(NULL != m_actualDetector);
    m_cameraFps = 25;
}

void TestActualDetector::cleanupTestCase()
{
    m_actualDetector->deleteLater();
    m_mainWindow->deleteLater();
    m_camera->deleteLater();
    m_config->deleteLater();
}

void TestActualDetector::cameraFrameConsumerThreadFunc() {
    m_cameraFrameConsumerRunning = true;
    while (m_consumeCameraFrames) {
        cv::Mat frame = m_camera->getWebcamFrame();
        m_cameraFramesConsumed++;
    }
    m_cameraFrameConsumerRunning = false;
}

void TestActualDetector::constructor()
{
    QVERIFY(NULL != m_actualDetector->m_mainWindow);
    QVERIFY(NULL != m_actualDetector->m_config);
    QVERIFY(NULL != m_actualDetector->camPtr);
    QVERIFY(m_mainWindow == m_actualDetector->m_mainWindow);
    QVERIFY(m_config == m_actualDetector->m_config);
    QVERIFY(m_camera == m_actualDetector->camPtr);
}

void TestActualDetector::mockCameraBlockNextFrame() {
    m_cameraFramesConsumed = 0;
    m_consumeCameraFrames = true;
    m_cameraFrameConsumerRunning = false;
    mockCamera_setFrameBlockingEnabled(true);

    m_cameraFrameConsumerThread = new std::thread(&TestActualDetector::cameraFrameConsumerThreadFunc, this);
    // wait camera frame consumer thread up
    while (!m_cameraFrameConsumerRunning) {
        std::this_thread::sleep_for(chrono::milliseconds(1));
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(chrono::milliseconds(10));

    mockCamera_releaseNextFrame();
    std::this_thread::sleep_for(chrono::milliseconds(10));

    QVERIFY(1 == m_cameraFramesConsumed);

    m_consumeCameraFrames = false;
    mockCamera_setFrameBlockingEnabled(false);
    m_cameraFrameConsumerThread->join();
    QVERIFY(!m_cameraFrameConsumerRunning);
    // there will be one more frame
    QVERIFY(2 == m_cameraFramesConsumed);
}

void TestActualDetector::detectBrightObjects() {
    int numFrames = 25;
    int objectRadius = 0;           // object radius in pixels
    int objectBrightness = 255;     // brightness value (0-255)
    int backgroundLightness = 127;   // lightness value (0-255)
    int objectX = 0;
    int objectY = 0;
    cv::Scalar objectColor;
    cv::Scalar backgroundColor;
    QFile detectionAreaFile(m_config->detectionAreaFile());

    // case: 50% lightness in background, 100% brightness in one small object,
    // object moving left to right in the middle area of video

    objectRadius = 5;
    objectY = m_config->cameraHeight() / 2;
    mockRecorderStartCount = 0;
    mockRecorderStopCount = 0;
    objectColor = cv::Scalar(objectBrightness, objectBrightness, objectBrightness);
    backgroundColor = cv::Scalar(backgroundLightness, backgroundLightness, backgroundLightness);

    if (!detectionAreaFile.exists()) {
        makeDetectionAreaFile();
    }
    mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3, backgroundColor);

    qDebug() << "Test starting actual detector";
    // ActualDetector::start() uses getCameraFrame() outside threads so only enable blocking
    // just before threads start. Luckily, progress value tells when it can be done.
    connect(m_actualDetector, SIGNAL(progressValueChanged(int)), this, SLOT(onActualDetectorStartProgressChanged(int)));
    QVERIFY(m_actualDetector->start());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (objectX = 0; objectX < m_config->cameraWidth(); objectX += (m_config->cameraWidth() / numFrames)) {
        mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3, backgroundColor);
        cv::circle(mockCameraNextFrame, Point(objectX, objectY), objectRadius, objectColor, -1);
        mockCamera_releaseNextFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m_cameraFps));
    }

    qDebug() << "Test stopping actual detector";
    mockCamera_setFrameBlockingEnabled(false);
    mockCamera_releaseNextFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    m_actualDetector->stopThread();

    QVERIFY(!m_actualDetector->m_recorder->m_recording);
    // verify recorder started
    QVERIFY(mockRecorderStartCount > 0);
    QVERIFY(mockRecorderStopCount > 0);
}

void TestActualDetector::detectDarkObjects() {
    int numFrames = 25;
    int objectRadius = 5;           // object radius in pixels
    int objectBrightness = 0;     // brightness value (0-255)
    int backgroundLightness = 127;   // lightness value (0-255)
    int objectX = 0;
    int objectY = 0;
    cv::Scalar objectColor;
    cv::Scalar backgroundColor;
    QFile detectionAreaFile(m_config->detectionAreaFile());

    // case: 50% lightness in background, 0% brightness in one small object,
    // object moving left to right in the middle area of video

    objectY = m_config->cameraHeight() / 2;
    mockRecorderStartCount = 0;
    mockRecorderStopCount = 0;
    objectColor = cv::Scalar(objectBrightness, objectBrightness, objectBrightness);
    backgroundColor = cv::Scalar(backgroundLightness, backgroundLightness, backgroundLightness);

    if (!detectionAreaFile.exists()) {
        makeDetectionAreaFile();
    }
    mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3, backgroundColor);

    connect(m_actualDetector, SIGNAL(progressValueChanged(int)), this, SLOT(onActualDetectorStartProgressChanged(int)));
    QVERIFY(m_actualDetector->start());

    for (objectX = 0; objectX < m_config->cameraWidth(); objectX += (m_config->cameraWidth() / numFrames)) {
        mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3, backgroundColor);
        cv::circle(mockCameraNextFrame, Point(objectX, objectY), objectRadius, objectColor, -1);
        mockCamera_releaseNextFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m_cameraFps));
    }

    mockCamera_setFrameBlockingEnabled(false);
    mockCamera_releaseNextFrame();
    m_actualDetector->stopThread();

    QVERIFY(!m_actualDetector->m_recorder->m_recording);
    // verify recorded didn't start
    QVERIFY(0 == mockRecorderStartCount);
    // stop is called always
    QVERIFY(mockRecorderStopCount > 0);
}

void TestActualDetector::makeDetectionAreaFile() {
    QFile detectionAreaFile(m_config->detectionAreaFile());
    QVERIFY(detectionAreaFile.open(QFile::ReadWrite));
    QTextStream out(&detectionAreaFile);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<UFOID>\n";
    for (int y = 0; y < m_config->cameraHeight(); y++) {
        for (int x = 0; x < m_config->cameraWidth(); x++) {
            out << "\t<point x=\"" << x << "\" y=\"" << y << "\"/>\n";
        }
    }
    out << "</UFOID>";
    detectionAreaFile.close();
}

void TestActualDetector::onActualDetectorStartProgressChanged(int progress) {
    if (90 == progress) {
        mockCamera_setFrameBlockingEnabled(true);
    }
}

QTEST_MAIN(TestActualDetector)

#include "testActualDetector.moc"
