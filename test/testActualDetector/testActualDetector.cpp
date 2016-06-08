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

QTEST_MAIN(TestActualDetector)

#include "testActualDetector.moc"
