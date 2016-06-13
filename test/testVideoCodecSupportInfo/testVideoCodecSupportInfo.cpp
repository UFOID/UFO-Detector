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

#include "videocodecsupportinfo.h"
#include <QString>
#include <QtTest>
#include <QRegExp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

extern int mockConfigResultVideoCodec;

class TestVideoCodecSupportInfo : public QObject
{
    Q_OBJECT

public:
    TestVideoCodecSupportInfo();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void constructor();
    void testOpencvSupport();
    void testEncoderSupport();
    void initialize();  // init() would be called for each test case
    void isSupportedMethods();

private:
    VideoCodecSupportInfo* m_codecInfo;
    QString m_videoEncoderLocation;
    QString m_testFileName;
    int m_frameWidth;
    int m_frameHeight;
    bool localTestOpencvSupport(int fourcc);
    bool localTestEncoderSupport(QString encoderCodecStr);
};

TestVideoCodecSupportInfo::TestVideoCodecSupportInfo()
{
    m_codecInfo = NULL;
    m_testFileName = "dummy.avi";
    m_videoEncoderLocation = "";
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    m_videoEncoderLocation = "/usr/bin/avconv";
#endif
    QVERIFY(m_videoEncoderLocation != "");
    m_frameWidth = 640;
    m_frameHeight = 480;
}

void TestVideoCodecSupportInfo::initTestCase() {
    m_codecInfo = new VideoCodecSupportInfo(m_videoEncoderLocation);
    QVERIFY(NULL != m_codecInfo);
}

void TestVideoCodecSupportInfo::cleanupTestCase() {
    m_codecInfo->deleteLater();
}

void TestVideoCodecSupportInfo::constructor() {
    QVERIFY(!m_codecInfo->m_isInitialized);
    QVERIFY(!m_codecInfo->isInitialized());
}

bool TestVideoCodecSupportInfo::localTestOpencvSupport(int fourcc) {
    cv::VideoWriter writer;
    cv::Mat frame;
    bool supported = false;
    int writtenFourcc = 0;
    std::string testFileNameStd(m_testFileName.toLocal8Bit().data());

    // try opening & writing file
    if (!writer.open(testFileNameStd, fourcc, 25, cv::Size(m_frameWidth, m_frameHeight))) {
        return false;
    }
    if (!writer.isOpened()) {
        return false;
    }
    frame = cv::Mat(m_frameHeight, m_frameWidth, CV_8UC3);
    writer.write(frame);
    writer.release();

    // check result
    cv::VideoCapture reader;
    reader.open(testFileNameStd);
    if (!reader.isOpened()) {
        return false;
    }
    writtenFourcc = (int)reader.get(CV_CAP_PROP_FOURCC);
    if (writtenFourcc == fourcc) {
        supported = true;
    }
    QFile testFile(m_testFileName);
    testFile.remove();
    return supported;
}

void TestVideoCodecSupportInfo::testOpencvSupport() {
    int rawFourcc = CV_FOURCC('I', 'Y', 'U', 'V');
    int ffv1Fourcc = CV_FOURCC('F', 'F', 'V', '1');
    int lagarithFourcc = CV_FOURCC('L', 'A', 'G', 'S');
    bool rawSupported = localTestOpencvSupport(rawFourcc);
    bool ffv1Supported = localTestOpencvSupport(ffv1Fourcc);
    bool lagarithSupported = localTestOpencvSupport(lagarithFourcc);

    QCOMPARE(m_codecInfo->testOpencvSupport(rawFourcc), rawSupported);
    QCOMPARE(m_codecInfo->testOpencvSupport(ffv1Fourcc), ffv1Supported);
    QCOMPARE(m_codecInfo->testOpencvSupport(lagarithFourcc), lagarithSupported);

    // These apply to basic/non-PPA OpenCV2 packages in Ubuntu 14.04.4 (Trusty)
    //QVERIFY(rawSupported);
    //QVERIFY(!ffv1Supported);
    //QVERIFY(!lagarithSupported);
}

bool TestVideoCodecSupportInfo::localTestEncoderSupport(QString encoderCodecStr) {
    QProcess encoder;
    QStringList encoderOutput;
    QStringList args;
    args << "-codecs";
    encoder.start(m_videoEncoderLocation, args);
    encoder.waitForFinished();
    while (encoder.canReadLine()) {
        encoderOutput << encoder.readLine();
    }
    QStringListIterator listIt(encoderOutput);
    while (listIt.hasNext()) {
        QString line = listIt.next();
        // Find encoder name and verify it can encode videos (E = encode, V = video).
        // Check the output of avconv/ffmpeg -codecs for more info.
        QRegExp regex("^.EV...\\ " + encoderCodecStr + ".*$");
        if (regex.exactMatch(line)) {
            //qDebug() << "Found:" << line;
            return true;
        }
    }
    return false;
}

void TestVideoCodecSupportInfo::testEncoderSupport() {
    qint64 startTime;
    qint64 endTime;
    qint64 difference;
    int rawFourcc = CV_FOURCC('I', 'Y', 'U', 'V');
    int ffv1Fourcc = CV_FOURCC('F', 'F', 'V', '1');
    int lagarithFourcc = CV_FOURCC('L', 'A', 'G', 'S');

    startTime = QDateTime::currentMSecsSinceEpoch();
    bool rawSupported = localTestEncoderSupport("rawvideo");
    bool ffv1Supported = localTestEncoderSupport("ffv1");
    bool lagarithSupported = localTestEncoderSupport("lagarith");
    endTime = QDateTime::currentMSecsSinceEpoch();
    difference = endTime - startTime;
    qDebug() << "Checking encoder support status inside test took" << difference << "ms";

    // case: encoder exists

    startTime = QDateTime::currentMSecsSinceEpoch();
    QCOMPARE(m_codecInfo->testEncoderSupport(rawFourcc), rawSupported);
    QCOMPARE(m_codecInfo->testEncoderSupport(ffv1Fourcc), ffv1Supported);
    QCOMPARE(m_codecInfo->testEncoderSupport(lagarithFourcc), lagarithSupported);
    endTime = QDateTime::currentMSecsSinceEpoch();
    difference = endTime - startTime;
    qDebug() << "Checking encoder support status by VideoCodecSupportInfo took" << difference << "ms";

    // case: encoder doesn't exist

    QString nonexistingEncoderLocation = "/nonexistingEncoderDir/nonexistingEncoderBin";
    QFile nonexistingEncoder(nonexistingEncoderLocation);
    QVERIFY(!nonexistingEncoder.exists());
    m_codecInfo->m_videoEncoderLocation = nonexistingEncoderLocation;
    QVERIFY(!m_codecInfo->testEncoderSupport(rawFourcc));
    QVERIFY(!m_codecInfo->testEncoderSupport(ffv1Fourcc));
    QVERIFY(!m_codecInfo->testEncoderSupport(lagarithFourcc));
    m_codecInfo->m_videoEncoderLocation = m_videoEncoderLocation;

    // These apply to basic/non-PPA OpenCV2 packages in Ubuntu 14.04.4 (Trusty)
    //QVERIFY(rawFourcc);
    //QVERIFY(ffv1Supported);
    //QVERIFY(!lagarithSupported);
}

void TestVideoCodecSupportInfo::initialize() {
    QMap<int, QList<int>> codecSupport;
    QList<int> codecs;
    codecs << CV_FOURCC('I', 'Y', 'U', 'V');
    codecs << CV_FOURCC('F', 'F', 'V', '1');
    codecs << CV_FOURCC('L', 'A', 'G', 'S');
    QListIterator<int> codecIt(codecs);
    int codec = 0;
    QMap<int, QString> encoderCodecStrMap;
    encoderCodecStrMap[CV_FOURCC('I', 'Y', 'U', 'V')] = "rawvideo";
    encoderCodecStrMap[CV_FOURCC('F', 'F', 'V', '1')] = "ffv1";
    encoderCodecStrMap[CV_FOURCC('L', 'A', 'G', 'S')] = "lagarith";

    // fill expected results
    while (codecIt.hasNext()) {
        codec = codecIt.next();
        codecSupport.insert(codec, QList<int>());
    }
    codecIt.toFront();
    while (codecIt.hasNext()) {
        codec = codecIt.next();
        QList<int> encoderList;
        if (localTestOpencvSupport(codec)) {
            encoderList = codecSupport.value(codec);
            encoderList.append(VideoCodecSupportInfo::OpenCv);
            codecSupport.insert(codec, encoderList);
            QVERIFY(codecSupport.value(codec).contains(VideoCodecSupportInfo::OpenCv));
        }
        if (localTestEncoderSupport(encoderCodecStrMap.value(codec))) {
            encoderList = codecSupport.value(codec);
            encoderList.append(VideoCodecSupportInfo::External);
            codecSupport.insert(codec, encoderList);
            QVERIFY(codecSupport.value(codec).contains(VideoCodecSupportInfo::External));
        }
    }

    QVERIFY(!m_codecInfo->m_isInitialized);
    QVERIFY(!m_codecInfo->isInitialized());
    QVERIFY(m_codecInfo->init());
    QVERIFY(m_codecInfo->m_isInitialized);
    QVERIFY(m_codecInfo->isInitialized());

    // check results
    codecIt.toFront();
    while (codecIt.hasNext()) {
        codec = codecIt.next();
        bool expectedOpencvSupported = codecSupport.value(codec).contains(VideoCodecSupportInfo::OpenCv);
        bool actualOpencvSupported = m_codecInfo->m_codecSupport.value(codec).contains(VideoCodecSupportInfo::OpenCv);
        QCOMPARE(actualOpencvSupported, expectedOpencvSupported);

        bool expectedEncoderSupported = codecSupport.value(codec).contains(VideoCodecSupportInfo::External);
        bool actualEncoderSupported = m_codecInfo->m_codecSupport.value(codec).contains(VideoCodecSupportInfo::External);
        QCOMPARE(actualEncoderSupported, expectedEncoderSupported);
    }
}

void TestVideoCodecSupportInfo::isSupportedMethods() {
    QMap<int, QList<int>> codecSupport;
    QList<int> codecs;
    codecs << CV_FOURCC('I', 'Y', 'U', 'V');
    codecs << CV_FOURCC('F', 'F', 'V', '1');
    codecs << CV_FOURCC('L', 'A', 'G', 'S');
    QListIterator<int> codecIt(codecs);
    int codec = 0;
    QMap<int, QString> encoderCodecStrMap;
    encoderCodecStrMap[CV_FOURCC('I', 'Y', 'U', 'V')] = "rawvideo";
    encoderCodecStrMap[CV_FOURCC('F', 'F', 'V', '1')] = "ffv1";
    encoderCodecStrMap[CV_FOURCC('L', 'A', 'G', 'S')] = "lagarith";

    // fill expected results
    while (codecIt.hasNext()) {
        codec = codecIt.next();
        codecSupport.insert(codec, QList<int>());
    }
    codecIt.toFront();
    while (codecIt.hasNext()) {
        codec = codecIt.next();
        QList<int> encoderList;
        if (localTestOpencvSupport(codec)) {
            encoderList = codecSupport.value(codec);
            encoderList.append(VideoCodecSupportInfo::OpenCv);
            codecSupport.insert(codec, encoderList);
            QVERIFY(codecSupport.value(codec).contains(VideoCodecSupportInfo::OpenCv));
        }
        if (localTestEncoderSupport(encoderCodecStrMap.value(codec))) {
            encoderList = codecSupport.value(codec);
            encoderList.append(VideoCodecSupportInfo::External);
            codecSupport.insert(codec, encoderList);
            QVERIFY(codecSupport.value(codec).contains(VideoCodecSupportInfo::External));
        }
    }

    m_codecInfo->init();

    // check results
    codecIt.toFront();
    while (codecIt.hasNext()) {
        codec = codecIt.next();
        bool expectedOpencvSupported = codecSupport.value(codec).contains(VideoCodecSupportInfo::OpenCv);
        bool actualOpencvSupported = m_codecInfo->isOpencvSupported(codec);
        QCOMPARE(actualOpencvSupported, expectedOpencvSupported);

        bool expectedEncoderSupported = codecSupport.value(codec).contains(VideoCodecSupportInfo::External);
        bool actualEncoderSupported = m_codecInfo->isEncoderSupported(codec);
        QCOMPARE(actualEncoderSupported, expectedEncoderSupported);

        if (expectedOpencvSupported || expectedEncoderSupported) {
            QVERIFY(m_codecInfo->supportedCodecs().contains(codec));
        } else {
            QVERIFY(!m_codecInfo->supportedCodecs().contains(codec));
        }
    }
}

QTEST_MAIN(TestVideoCodecSupportInfo)

#include "testVideoCodecSupportInfo.moc"
