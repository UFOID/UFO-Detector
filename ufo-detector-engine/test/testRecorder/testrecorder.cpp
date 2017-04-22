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

#include "recorder.h"
#include "camera.h"
#include "config.h"
#include "datamanager.h"
#include <QtTest>
#include <QString>
#include <QFile>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

extern cv::Mat mockCameraNextFrame;
extern int mockConfigResultVideoCodec;
extern QString mockConfigResultVideoCodecStr;

/**
 * @brief Unit test of Recorder class
 */
class TestRecorder : public QObject
{
    Q_OBJECT

public:
    TestRecorder();
    ~TestRecorder();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void constructor();

    /*
     * Go through supported codecs and check Recorder will encode video with correct codec.
     */
    void videoCodecs();

    /*
     * Basic mock Camera test. See a blocking Camera::getWebcamFrame() test in ActualDetector unit test.
     */
    void mockCamera();

    void saveVideoThumbnailImage();

private:
    Recorder* m_recorder;
    Config* m_config;
    Camera* m_camera;
    DataManager* m_dataManager;
    int m_requestEncodingCounter;
    QString m_requestEncodingTempFileName;
    QString m_requestEncodingTargetFileName;

    void fourccToStr(int fourcc, char str[5]);

private slots:
    void onVideoEncodingRequested(QString tempFileName, QString targetFileName);
};

TestRecorder::TestRecorder() {
    m_requestEncodingCounter = 0;
}

TestRecorder::~TestRecorder() {
}

void TestRecorder::initTestCase() {
    m_config = new Config();
    m_camera = new Camera(m_config->cameraIndex(), m_config->cameraWidth(), m_config->cameraHeight());
    m_dataManager = new DataManager(m_config);
    m_recorder = new Recorder(m_camera, m_config, m_dataManager);
}

void TestRecorder::cleanupTestCase() {
    m_recorder->deleteLater();
    m_camera->deleteLater();
    m_config->deleteLater();
    QDir videoDir(m_config->resultVideoDir());
    QVERIFY(videoDir.removeRecursively());
    QVERIFY(!videoDir.exists());
}

void TestRecorder::constructor() {
    QVERIFY(NULL != m_config);
    QVERIFY(NULL != m_camera);
    QVERIFY(NULL != m_recorder);
    QCOMPARE(m_camera, m_recorder->m_camera);
    QCOMPARE(m_config, m_recorder->m_config);

    cv::Size expectedVideoResolution(m_config->cameraWidth(), m_config->cameraHeight());
    double cameraAspectRatio = (double)m_config->cameraWidth() / (double)m_config->cameraHeight();
    int thumbSideLength = 80;
    QCOMPARE(m_recorder->m_defaultThumbnailSideLength, thumbSideLength);
    cv::Size expectedThumbnailResolution(thumbSideLength,
            qBound(thumbSideLength / 2, (int)(thumbSideLength / cameraAspectRatio), thumbSideLength));
    QCOMPARE(m_recorder->m_videoResolution, expectedVideoResolution);
    QCOMPARE(m_recorder->m_thumbnailResolution, expectedThumbnailResolution);

    QCOMPARE(m_recorder->m_drawRectangles, m_config->resultVideoWithObjectRectangles());
    QCOMPARE(m_recorder->m_objectPositiveColor, cv::Scalar(255, 0, 0));
    QCOMPARE(m_recorder->m_objectNegativeColor, cv::Scalar(0, 0, 255));
    QCOMPARE(m_recorder->m_objectRectangleColor, m_recorder->m_objectPositiveColor);

    QCOMPARE(m_recorder->m_resultVideoDirName, m_config->resultVideoDir());
    QCOMPARE(m_recorder->m_videoFileExtension, QString(".avi"));
    QCOMPARE(m_recorder->m_thumbnailExtension, QString(".jpg"));
    QCOMPARE(m_recorder->m_thumbnailDirName, QString("thumbnails"));
    QDir thumbDir(m_config->resultVideoDir() + "/" + m_recorder->m_thumbnailDirName);
    QVERIFY(thumbDir.exists());

    QVERIFY(!m_recorder->m_willSaveVideo);
    QVERIFY(!m_recorder->m_recording);
}

void TestRecorder::mockCamera() {
    Vec3b pixel;
    bool showImage = false;     // If you want to see test images, change this to true

    cv::namedWindow("Mock camera frame");

    QVERIFY(m_camera->getWebcamFrame().data == mockCameraNextFrame.data);
    QVERIFY(mockCameraNextFrame.empty());
    QVERIFY(m_camera->getWebcamFrame().empty());
    QVERIFY(0 == mockCameraNextFrame.cols);
    QVERIFY(0 == mockCameraNextFrame.rows);
    QVERIFY(0 == m_camera->getWebcamFrame().cols);
    QVERIFY(0 == m_camera->getWebcamFrame().rows);

    // 1st image
    mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3, Scalar(0, 0, 0));
    cv::rectangle(mockCameraNextFrame, Rect(10, 10, 50, 50), Scalar(40, 60, 80), -1);
    if (showImage) {
        cv::imshow("Mock camera frame", mockCameraNextFrame);
        cv::waitKey(0);
    }
    QVERIFY(m_camera->getWebcamFrame().data == mockCameraNextFrame.data);
    QVERIFY(!mockCameraNextFrame.empty());
    QVERIFY(!m_camera->getWebcamFrame().empty());
    QVERIFY(m_config->cameraWidth() == m_camera->getWebcamFrame().cols);
    QVERIFY(m_config->cameraHeight() == m_camera->getWebcamFrame().rows);
    pixel = m_camera->getWebcamFrame().at<Vec3b>(Point(10, 10));
    QVERIFY(40 == pixel[0]);
    QVERIFY(60 == pixel[1]);
    QVERIFY(80 == pixel[2]);
    pixel = m_camera->getWebcamFrame().at<Vec3b>(Point(9, 10));
    QVERIFY(0 == pixel[0]);
    QVERIFY(0 == pixel[1]);
    QVERIFY(0 == pixel[2]);

    // another image
    mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3, Scalar(0, 0, 0));
    cv::rectangle(mockCameraNextFrame, Rect(110, 10, 50, 50), Scalar(90, 70, 50), -1);
    if (showImage) {
        cv::imshow("Mock camera frame", mockCameraNextFrame);
        cv::waitKey(0);
    }
    QVERIFY(m_camera->getWebcamFrame().data == mockCameraNextFrame.data);
    pixel = m_camera->getWebcamFrame().at<Vec3b>(Point(110, 10));
    QVERIFY(90 == pixel[0]);
    QVERIFY(70 == pixel[1]);
    QVERIFY(50 == pixel[2]);
    pixel = m_camera->getWebcamFrame().at<Vec3b>(Point(109, 10));
    QVERIFY(0 == pixel[0]);
    QVERIFY(0 == pixel[1]);
    QVERIFY(0 == pixel[2]);
}

void TestRecorder::videoCodecs()
{
    int expectedFourcc = 0;
    int actualFourcc = 0;
    QStringList codecs;
    QString codecStr = "";
    int codec = 0;
    bool codecSupported = false;
    VideoCodecSupportInfo* codecInfo = m_config->videoCodecSupportInfo();

    // FOURCC codes for video codecs: http://www.fourcc.org/codecs.php
    codecs << "IYUV" << "FFV1" << "LAGS";

    for (int i = 0; i < codecs.size(); i++) {
        if (m_recorder) {
            m_recorder->disconnect(m_recorder, SIGNAL(videoEncodingRequested(QString,QString)), this,
                                   SLOT(onVideoEncodingRequested(QString,QString)));
            m_recorder->deleteLater();
        }

        codecStr = codecs[i];
        codec = m_config->videoCodecSupportInfo()->stringToFourcc(codecStr);
        codecSupported = codecInfo->supportedCodecs().contains(codec);
        expectedFourcc = codec;
        mockConfigResultVideoCodecStr = codecStr;
        mockConfigResultVideoCodec = codec;
        QVERIFY(m_config->resultVideoCodecStr() == codecStr);
        QVERIFY(m_config->resultVideoCodec() == codec);

        qDebug() << "=== Testing codec" << codecStr << "===";

        m_recorder = new Recorder(m_camera, m_config, m_dataManager);
        connect(m_recorder, SIGNAL(videoEncodingRequested(QString,QString)), this,
                SLOT(onVideoEncodingRequested(QString,QString)));

        cv::Mat firstFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3);
        // Recorder will be getting mockCameraNextFrame
        mockCameraNextFrame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3);

        // expected video file name
        /// @todo must allow timestamp delta (Recorder can start a bit later so uses different timestamp)
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss");
        QString videoFileExt = ".avi";
        QString filenameTemp = m_config->resultVideoDir() + "/Capture--" + timestamp + "temp" + videoFileExt;
        QString filenameFinal = m_config->resultVideoDir() + "/Capture--" + timestamp + videoFileExt;
        QFile tempFile(filenameTemp);
        QFile resultVideoFile(filenameFinal);
        qDebug() << "target file" << resultVideoFile.fileName();

        m_requestEncodingTempFileName = "";
        m_requestEncodingTargetFileName = "";
        m_requestEncodingCounter = 0;

        m_recorder->startRecording(firstFrame);
        QTest::qWait(100);

        QVERIFY(tempFile.exists());
        QVERIFY(!resultVideoFile.exists());

        QTest::qWait(400);
        m_recorder->stopRecording(true);
        QVERIFY(m_recorder->m_willSaveVideo);
        // codec only supported by ffmpeg/avconv, not by OpenCV
        if (!codecInfo->isOpencvSupported(codec) && codecInfo->isEncoderSupported(codec)) {
            QTest::qWait(2000);
            QCOMPARE(m_requestEncodingTempFileName, filenameTemp);
            QCOMPARE(m_requestEncodingTargetFileName, filenameFinal);
            QCOMPARE(m_requestEncodingCounter, 1);
        } else {
            QTest::qWait(500);
        }

        QVERIFY(!tempFile.exists());
        QVERIFY(resultVideoFile.exists());

        // check video has correct codec
        cv::VideoCapture videoFile;
        QVERIFY(videoFile.open(filenameFinal.toStdString()));
        actualFourcc = (int)videoFile.get(CV_CAP_PROP_FOURCC);
        videoFile.release();

        char fourccStr[5];
        for (int i=0; i < 5; i++) {
            fourccStr[i] = 0;
        }
        fourccToStr(actualFourcc, fourccStr);
        qDebug() << "Actual codec:" << QString(fourccStr);
        if (codecSupported) {
            QVERIFY(actualFourcc == expectedFourcc);
        } else {
            // non-supported codec: fall back to raw video codec
            QVERIFY(actualFourcc == codecInfo->stringToFourcc("IYUV"));
        }
    }
    // Recorder has modified the next frame of mock Camera so clean it
    mockCameraNextFrame = cv::Mat();
}

void TestRecorder::saveVideoThumbnailImage() {
    QString dateTime = "2017-04-10--12-00-00";
    QString thumbnailFileName = m_config->resultVideoDir() + "/thumbnails/" + dateTime + ".jpg";
    QFile thumbnailFile(thumbnailFileName);
    Mat thumbnailImage(100, 200, CV_8UC3);

    m_recorder->saveVideoThumbnailImage(thumbnailImage, dateTime);

    QVERIFY(thumbnailFile.exists());
    QVERIFY(thumbnailFile.remove());
    QVERIFY(!thumbnailFile.exists());
}

void TestRecorder::fourccToStr(int fourcc, char str[5]) {
    for (int i=0; i < 4; i++) {
        str[i] = (fourcc >> (i*8)) & 0xFF;
    }
    str[4] = '\0';
}

void TestRecorder::onVideoEncodingRequested(QString tempFileName, QString targetFileName) {
    m_requestEncodingTempFileName = tempFileName;
    m_requestEncodingTargetFileName = targetFileName;
    m_requestEncodingCounter++;
}

QTEST_MAIN(TestRecorder)

#include "testrecorder.moc"
