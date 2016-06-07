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
#include "actualdetector.h"
#include "camera.h"
#include "config.h"
#include "mainwindow.h"
#include <QtTest>
#include <QString>
#include <QFile>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

extern cv::Mat mockCameraNextFrame;

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
    void saveResultData();

    /**
     * Basic mock Camera test. See a blocking Camera::getWebcamFrame() test in ActualDetector unit test.
     */
    void mockCamera();

private:
    Recorder* m_recorder;
    Config* m_config;
    Camera* m_camera;
    QFile m_resultDataFile;
    int m_updateVideoWidgetCallCounter;

private slots:
    void onResultDataSaved(QString fileName, QString dateTime, QString videoLength);
};

TestRecorder::TestRecorder() {
    m_updateVideoWidgetCallCounter = 0;
}

TestRecorder::~TestRecorder() {
}

void TestRecorder::onResultDataSaved(QString fileName, QString dateTime, QString videoLength) {
    Q_UNUSED(fileName);
    Q_UNUSED(dateTime);
    Q_UNUSED(videoLength);
    m_updateVideoWidgetCallCounter++;
}

void TestRecorder::initTestCase() {
    m_config = new Config();

    // create result data file here because it is created in MainWindow and not by Recorder
    m_resultDataFile.setFileName(m_config->resultDataFile());
    if (m_resultDataFile.exists()) {
        m_resultDataFile.remove();
    }
    QDomDocument resultDataDom;
    QVERIFY(!m_resultDataFile.exists());
    QVERIFY(m_resultDataFile.open(QFile::ReadWrite));
    QTextStream stream(&m_resultDataFile);
    QDomElement ufoidElement = resultDataDom.createElement("UFOID");
    resultDataDom.appendChild(ufoidElement);
    stream << resultDataDom.toString();
    m_resultDataFile.flush();
    m_resultDataFile.close();
    QVERIFY(m_resultDataFile.exists());

    m_camera = new Camera(m_config->cameraIndex(), m_config->cameraWidth(), m_config->cameraHeight());
    m_recorder = new Recorder(m_camera, m_config);
}

void TestRecorder::cleanupTestCase() {
    m_recorder->deleteLater();
    m_camera->deleteLater();
    m_config->deleteLater();
    QVERIFY(m_resultDataFile.remove());
    QVERIFY(!m_resultDataFile.exists());
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

    QCOMPARE(m_recorder->m_resultDataFile.fileName(), m_config->resultDataFile());
    QCOMPARE(m_recorder->m_resultVideoDirName, m_config->resultVideoDir());
    QCOMPARE(m_recorder->m_videoFileExtension, QString(".avi"));
    QCOMPARE(m_recorder->m_thumbnailExtension, QString(".jpg"));
    QCOMPARE(m_recorder->m_thumbnailDirName, QString("thumbnails"));
    QFile resultDataFile(m_config->resultDataFile());
    QVERIFY(resultDataFile.exists());
    QDir thumbDir(m_config->resultVideoDir() + "/" + m_recorder->m_thumbnailDirName);
    QVERIFY(thumbDir.exists());

    QVERIFY(!m_recorder->m_willSaveVideo);
    QVERIFY(!m_recorder->m_recording);
}

void TestRecorder::saveResultData() {
    QString dateTime = "2016-05-22--12-00-00";
    QString videoLength = "01:02";
    QDomDocument resultDataDom;
    QDomElement videoEntry;
    connect(m_recorder, SIGNAL(resultDataSaved(QString,QString,QString)),
            this, SLOT(onResultDataSaved(QString,QString,QString)));

    // case: write one entry

    // check result data file doesn't have any other content than the main tag
    QVERIFY(m_resultDataFile.open(QFile::ReadOnly));
    QVERIFY(resultDataDom.setContent(m_resultDataFile.readAll()));
    m_resultDataFile.close();
    QVERIFY(resultDataDom.firstChild().childNodes().isEmpty());
    resultDataDom.clear();

    QCOMPARE(m_updateVideoWidgetCallCounter, 0);
    m_recorder->m_firstFrame = cv::Mat(cv::Size(m_config->cameraWidth(), m_config->cameraWidth()), CV_8UC3);

    m_recorder->saveResultData(dateTime, videoLength);

    QCOMPARE(m_updateVideoWidgetCallCounter, 1);
    QVERIFY(!m_recorder->m_resultDataFile.isOpen());
    // check content of result data file
    QVERIFY(m_resultDataFile.open(QFile::ReadOnly));
    QVERIFY(resultDataDom.setContent(m_resultDataFile.readAll()));
    m_resultDataFile.close();
    QVERIFY(!resultDataDom.firstChild().childNodes().isEmpty());
    QCOMPARE(resultDataDom.firstChild().childNodes().count(), 1);
    videoEntry = resultDataDom.firstChild().childNodes().at(0).toElement();
    QCOMPARE(videoEntry.nodeName(), QString("Video"));
    QCOMPARE(videoEntry.attributes().length(), 3);
    QCOMPARE(videoEntry.attribute("Pathname"), m_config->resultVideoDir());
    QCOMPARE(videoEntry.attribute("Length"), videoLength);
    QCOMPARE(videoEntry.attribute("DateTime"), dateTime);
    resultDataDom.clear();
    // check thumbnail
    QString thumbnailFileName = m_config->resultVideoDir() + "/thumbnails/" + dateTime + ".jpg";
    QFile thumbnailFile(thumbnailFileName);
    QVERIFY(thumbnailFile.exists());
    QVERIFY(thumbnailFile.remove());
    QVERIFY(!thumbnailFile.exists());
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

QTEST_APPLESS_MAIN(TestRecorder)

#include "testrecorder.moc"
