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
#include "config.h"
#include "camera.h"
#include "datamanager.h"
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QDir>

extern cv::Mat mockCameraNextFrame;
extern std::atomic<bool> isReadingVideo;
extern void mockCamera_setFrameBlockingEnabled(bool);
extern void mockCamera_releaseNextFrame();

extern int mockRecorderStartCount;
extern int mockRecorderStopCount;
extern QString testResourceFolder();

extern void startCameraFromVideo(QFile* videoFile);

class TestActualDetector : public QObject
{
    Q_OBJECT

public:
    TestActualDetector();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void constructor();
    void initialize();

    /**
     * Test blocking Camera::getWebcamFrame(). See a basic non-blocking test in Recorder unit test.
     */
    void mockCameraBlockNextFrame();

    void detectBrightObjects();
    void detectDarkObjects();
    void testBird();
    void setShowCameraVideo();


private:
    ActualDetector* m_actualDetector;
    Config* m_config;
    Camera* m_camera;
    DataManager* m_dataManager;

    // test thread to consume camera frames
    std::thread* m_cameraFrameConsumerThread;
    std::unique_ptr<std::thread> m_videoReaderThread;
    void cameraFrameConsumerThreadFunc();
    int m_cameraFramesConsumed; ///< number of camera frames received by cameraFrameConsumerThreadFunc
    bool m_consumeCameraFrames; ///< camera frame consumer thread run enabled flag
    bool m_cameraFrameConsumerRunning;  ///< camera frame consumer thread is running

    int m_cameraFps;    ///< frames per second for mock camera
    int m_cameraFrameUpdatedCounter; ///< how many times onActualDetectorCameraFrameUpdated has been called

    void makeDetectionAreaFile();

private slots:
    void onActualDetectorStartProgressChanged(int progress);
    void onActualDetectorCameraFrameUpdated(QImage pixmap);
};

TestActualDetector::TestActualDetector() {
    m_actualDetector = NULL;
    m_config = NULL;
    m_camera = NULL;
    m_dataManager = NULL;
    m_cameraFrameConsumerThread = NULL;
    m_cameraFramesConsumed = 0;
    m_consumeCameraFrames = false;
    m_cameraFrameConsumerRunning = false;
    m_cameraFps = 0;
    m_cameraFrameUpdatedCounter = 0;
}

void TestActualDetector::initTestCase() {
    m_config = new Config();
    QVERIFY(NULL != m_config);
    m_camera = new Camera(m_config->cameraIndex(), m_config->cameraWidth(), m_config->cameraHeight());
    QVERIFY(NULL != m_camera);
    m_dataManager = new DataManager(m_config);
    QVERIFY(NULL != m_dataManager);
    m_actualDetector = new ActualDetector(m_camera, m_config, m_dataManager);
    QVERIFY(NULL != m_actualDetector);
    m_cameraFps = 25;

    QFile detectionAreaFile(m_config->detectionAreaFile());
    if (!detectionAreaFile.exists()) {
        makeDetectionAreaFile();
    }
    QPolygon* detectionArea = new QPolygon();
    *detectionArea << QPoint(0, 0) << QPoint(0, m_config->cameraHeight() - 1)
                  << QPoint(m_config->cameraWidth() - 1, m_config->cameraHeight() - 1)
                  << QPoint(m_config->cameraWidth() - 1, 0);
    m_dataManager->m_detectionAreaPolygons << detectionArea;
}

void TestActualDetector::cleanupTestCase() {
    m_actualDetector->deleteLater();
    m_dataManager->deleteLater();
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

void TestActualDetector::constructor() {
    QVERIFY(NULL != m_actualDetector->m_config);
    QVERIFY(NULL != m_actualDetector->m_camPtr);
    QVERIFY(m_config == m_actualDetector->m_config);
    QVERIFY(m_camera == m_actualDetector->m_camPtr);
    QVERIFY(!m_actualDetector->m_showCameraVideo);
}

void TestActualDetector::initialize() {
    QVERIFY(m_actualDetector->m_currentFrame.empty());
    QVERIFY(m_actualDetector->m_nextFrame.empty());
    QVERIFY(m_actualDetector->m_prevFrame.empty());
    QVERIFY(m_actualDetector->m_resultFrame.empty());
    QVERIFY(!m_actualDetector->m_showCameraVideo);
    QVERIFY(!m_actualDetector->m_startedRecording);

    mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(),
                                  CV_8UC3, cv::Scalar(127, 127, 127));
    mockCamera_setFrameBlockingEnabled(false);
    m_actualDetector->m_showCameraVideo = true;

    QVERIFY(m_actualDetector->initialize());

    QVERIFY(!m_actualDetector->m_currentFrame.empty());
    QVERIFY(!m_actualDetector->m_nextFrame.empty());
    QVERIFY(!m_actualDetector->m_prevFrame.empty());
    QVERIFY(!m_actualDetector->m_resultFrame.empty());
    QVERIFY(!m_actualDetector->m_startedRecording);
    // must not change m_showCameraVideo
    QVERIFY(m_actualDetector->m_showCameraVideo);
    m_actualDetector->m_showCameraVideo = false;
}

void TestActualDetector::testBird(){
    QFile file(testResourceFolder()+"\\positive bird.avi");
    if (!file.exists())
        QSKIP("this test need a positive bird video");

    m_videoReaderThread.reset(new std::thread(startCameraFromVideo,&file));

    QVERIFY(m_actualDetector->start());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    QVERIFY(NULL != m_actualDetector->state);
    QSignalSpy spy(m_actualDetector->state, SIGNAL(foundDetectionResult(DetectorState::DetectionResult)));

    qDebug() << "Waiting for bird test video to finish";

    m_videoReaderThread->join();

    mockCamera_setFrameBlockingEnabled(false);
    mockCamera_releaseNextFrame();

    //Wait 1 sec to detector to recognize there is no movement and stop
    std::this_thread::sleep_for(chrono::seconds(1));
    m_actualDetector->stopThread();

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0) ==  DetectorState::DetectionResult::BIRD);



    qDebug () << "done";
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
    delete m_cameraFrameConsumerThread;
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

    // ActualDetector::start() uses getCameraFrame() outside threads so only enable blocking
    // just before threads start. Luckily, progress value tells when it can be done.
    connect(m_actualDetector, SIGNAL(progressValueChanged(int)), this,
            SLOT(onActualDetectorStartProgressChanged(int)));
    QVERIFY(m_actualDetector->start());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    QSignalSpy spy(m_actualDetector->state, SIGNAL(foundDetectionResult(DetectorState::DetectionResult)));

    for (objectX = 0; objectX < m_config->cameraWidth(); objectX += (m_config->cameraWidth() / numFrames)) {
        mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3, backgroundColor);
        cv::circle(mockCameraNextFrame, Point(objectX, objectY), objectRadius, objectColor, -1);
        mockCamera_releaseNextFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m_cameraFps));
    }

   // mockCameraNextFrame
    //Sleep for one second so the recording thread stops
    mockCamera_setFrameBlockingEnabled(false);
    std::this_thread::sleep_for(std::chrono::seconds(2));


    mockCamera_releaseNextFrame();
    m_actualDetector->stopThread();

    QVERIFY(!m_actualDetector->m_recorder->m_recording);
    // verify recorder started
    QVERIFY(mockRecorderStartCount > 0);
    QVERIFY(mockRecorderStopCount > 0);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0) ==  DetectorState::DetectionResult::UNKNOWN);

    disconnect(m_actualDetector, SIGNAL(progressValueChanged(int)), this,
            SLOT(onActualDetectorStartProgressChanged(int)));
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
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

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

    disconnect(m_actualDetector, SIGNAL(progressValueChanged(int)), this,
            SLOT(onActualDetectorStartProgressChanged(int)));
}

void TestActualDetector::setShowCameraVideo() {
    QVERIFY(false == m_actualDetector->m_showCameraVideo);
    m_actualDetector->setShowCameraVideo(true);
    QVERIFY(m_actualDetector->m_showCameraVideo);

    int numFrames = 24;
    cv::Scalar backgroundColor = cv::Scalar(127, 127, 127);
    QFile detectionAreaFile(m_config->detectionAreaFile());

    if (!detectionAreaFile.exists()) {
        makeDetectionAreaFile();
    }

    mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3, backgroundColor);
    mockCamera_setFrameBlockingEnabled(false);

    // queued signals don't arrive at the test so using direct connection
    qWarning() << "Testing frame update signal with Qt::DirectConnection";
    connect(m_actualDetector, SIGNAL(updatePixmap(QImage)), this,
            SLOT(onActualDetectorCameraFrameUpdated(QImage)), Qt::DirectConnection);

    // case: camera video is shown ( = frames emitted by signal)

    m_cameraFrameUpdatedCounter = 0;
    connect(m_actualDetector, SIGNAL(progressValueChanged(int)), this,
            SLOT(onActualDetectorStartProgressChanged(int)));

    QVERIFY(m_actualDetector->start());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (int i = 0; i < numFrames; i++) {
        mockCamera_releaseNextFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m_cameraFps));
        //qDebug() << "round " << (i+1) << " counter =" << m_cameraFrameUpdatedCounter;
        //QCOMPARE((i + 1) , m_cameraFrameUpdatedCounter);
        QVERIFY(m_actualDetector->m_showCameraVideo);
    }
    mockCamera_setFrameBlockingEnabled(false);
    mockCamera_releaseNextFrame();
    m_actualDetector->stopThread();

    // case: camera video is not shown (= no frames emitted by signal)

    m_actualDetector->setShowCameraVideo(false);
    m_cameraFrameUpdatedCounter = 0;

    QVERIFY(m_actualDetector->start());
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (int i = 0; i < numFrames; i++) {
        mockCamera_releaseNextFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m_cameraFps));
        // qDebug() << "round " << (i+1) << " counter =" << m_cameraFrameUpdatedCounter;
        QVERIFY(0 == m_cameraFrameUpdatedCounter);
        QVERIFY(!m_actualDetector->m_showCameraVideo);
    }
    mockCamera_setFrameBlockingEnabled(false);
    mockCamera_releaseNextFrame();
    m_actualDetector->stopThread();

    disconnect(m_actualDetector, SIGNAL(progressValueChanged(int)), this,
            SLOT(onActualDetectorStartProgressChanged(int)));
    disconnect(m_actualDetector, SIGNAL(updatePixmap(QImage)), this,
            SLOT(onActualDetectorCameraFrameUpdated(QImage)));
}

void TestActualDetector::makeDetectionAreaFile() {
    QFile detectionAreaFile(m_config->detectionAreaFile());
    QVERIFY(detectionAreaFile.open(QFile::ReadWrite));
    QTextStream out(&detectionAreaFile);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    out << "<detectionarealist>" << endl;
    out << "<detectionarea>" << endl;
    out << "<camera id=\"" << m_config->cameraIndex() << "\" width=\""
        << m_config->cameraWidth() << "\" height=\"" << m_config->cameraHeight()
        << "\"/>" << endl;
    out << "<polygon>" << endl;
    out << "<point x=\"0\" y=\"0\"/>" << endl;
    out << "<point x=\"0\" y=\"" << m_config->cameraHeight() << "\"/>" << endl;
    out << "<point x=\"" << m_config->cameraWidth() << "\" y=\"" << m_config->cameraHeight() << "\"/>" << endl;
    out << "<point x=\"" << m_config->cameraWidth() << "\" y=\"0\"/>" << endl;
    out << "</polygon>" << endl;
    out << "</detectionarea>" << endl;
    out << "</detectionarealist>" << endl;
    detectionAreaFile.close();
}

void TestActualDetector::onActualDetectorStartProgressChanged(int progress) {
    if (90 == progress) {
        mockCamera_setFrameBlockingEnabled(true);
    }
}

void TestActualDetector::onActualDetectorCameraFrameUpdated(QImage pixmap) {
    Q_UNUSED(pixmap);
    m_cameraFrameUpdatedCounter++;
}

QTEST_MAIN(TestActualDetector)

#include "testActualDetector.moc"
