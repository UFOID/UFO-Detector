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
    void codecName();
    void toFromFourcc();
    void rawVideoCodecStr();
    void removeSupport();

private:
    VideoCodecSupportInfo* m_codecInfo;
    QHash<int, QList<int>> m_expectedCodecs;   // expected supported codecs
    QHash<int, QString> m_expectedEncoderStrings;   // expected codec strings for encoder
    QString m_videoEncoderLocation;
    QString m_testFileName;
    int m_frameWidth;
    int m_frameHeight;
    bool localTestOpencvSupport(int fourcc);
    bool localTestEncoderSupport(QString encoderCodecStr);
    void fillExpectedCodecs();
};

TestVideoCodecSupportInfo::TestVideoCodecSupportInfo()
{
    m_codecInfo = NULL;
    m_testFileName = "dummy.avi";
    m_videoEncoderLocation = "";
    m_frameWidth = 640;
    m_frameHeight = 480;
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    m_videoEncoderLocation = "/usr/bin/avconv";
#elif defined(Q_OS_WIN)
    m_videoEncoderLocation = QCoreApplication::applicationDirPath()+"/ffmpeg.exe";
#endif
}

void TestVideoCodecSupportInfo::initTestCase() {
    QVERIFY(!m_videoEncoderLocation.isEmpty());
    QFile encoderExecutable(m_videoEncoderLocation);
    QVERIFY(encoderExecutable.exists());
    m_codecInfo = new VideoCodecSupportInfo(m_videoEncoderLocation);
    QVERIFY(NULL != m_codecInfo);
    fillExpectedCodecs();
}

void TestVideoCodecSupportInfo::cleanupTestCase() {
    m_codecInfo->deleteLater();
}

void TestVideoCodecSupportInfo::fillExpectedCodecs() {
    m_expectedCodecs.insert(CV_FOURCC('I', 'Y', 'U', 'V'), QList<int>());
    m_expectedCodecs.insert(CV_FOURCC('F', 'F', 'V', '1'), QList<int>());
    m_expectedCodecs.insert(CV_FOURCC('L', 'A', 'G', 'S'), QList<int>());

    m_expectedEncoderStrings[CV_FOURCC('I', 'Y', 'U', 'V')] = "rawvideo";
    m_expectedEncoderStrings[CV_FOURCC('F', 'F', 'V', '1')] = "ffv1";
    m_expectedEncoderStrings[CV_FOURCC('L', 'A', 'G', 'S')] = "lagarith";

    int codec = 0;
    QString codecStr;
    QListIterator<int> codecIt(m_expectedCodecs.keys());
    codecIt.toFront();
    qDebug() << "Codec support";
    while (codecIt.hasNext()) {
        codec = codecIt.next();
        codecStr = m_expectedEncoderStrings[codec];
        QList<int> encoderList;
        if (localTestOpencvSupport(codec)) {
            encoderList = m_expectedCodecs.value(codec);
            encoderList.append(VideoCodecSupportInfo::OpenCv);
            m_expectedCodecs.insert(codec, encoderList);
            QVERIFY(m_expectedCodecs.value(codec).contains(VideoCodecSupportInfo::OpenCv));
            qDebug() << codecStr << "in OpenCV: yes";
        } else {
            qDebug() << codecStr << "in OpenCV: no";
        }
        if (localTestEncoderSupport(m_expectedEncoderStrings.value(codec))) {
            encoderList = m_expectedCodecs.value(codec);
            encoderList.append(VideoCodecSupportInfo::External);
            m_expectedCodecs.insert(codec, encoderList);
            QVERIFY(m_expectedCodecs.value(codec).contains(VideoCodecSupportInfo::External));
            qDebug() << codecStr << "in ffmpeg/avconv: yes";
        } else {
            qDebug() << codecStr << "in ffmpeg/avconv: no";
        }
    }
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
    QListIterator<int> codecIt(m_expectedCodecs.keys());
    int codec = 0;

    QVERIFY(!m_codecInfo->m_isInitialized);
    QVERIFY(!m_codecInfo->isInitialized());
    QVERIFY(m_codecInfo->init());
    QVERIFY(m_codecInfo->m_isInitialized);
    QVERIFY(m_codecInfo->isInitialized());

    // check results
    codecIt.toFront();
    while (codecIt.hasNext()) {
        codec = codecIt.next();
        bool expectedOpencvSupported = m_expectedCodecs.value(codec).contains(VideoCodecSupportInfo::OpenCv);
        bool actualOpencvSupported = m_codecInfo->m_codecSupport.value(codec).contains(VideoCodecSupportInfo::OpenCv);
        QCOMPARE(actualOpencvSupported, expectedOpencvSupported);

        bool expectedEncoderSupported = m_expectedCodecs.value(codec).contains(VideoCodecSupportInfo::External);
        bool actualEncoderSupported = m_codecInfo->m_codecSupport.value(codec).contains(VideoCodecSupportInfo::External);
        QCOMPARE(actualEncoderSupported, expectedEncoderSupported);
    }
}

void TestVideoCodecSupportInfo::isSupportedMethods() {
    QListIterator<int> codecIt(m_expectedCodecs.keys());

    while (codecIt.hasNext()) {
        int codec = codecIt.next();
        bool expectedOpencvSupported = m_expectedCodecs.value(codec).contains(VideoCodecSupportInfo::OpenCv);
        bool actualOpencvSupported = m_codecInfo->isOpencvSupported(codec);
        QCOMPARE(actualOpencvSupported, expectedOpencvSupported);

        bool expectedEncoderSupported = m_expectedCodecs.value(codec).contains(VideoCodecSupportInfo::External);
        bool actualEncoderSupported = m_codecInfo->isEncoderSupported(codec);
        QCOMPARE(actualEncoderSupported, expectedEncoderSupported);

        if (expectedOpencvSupported || expectedEncoderSupported) {
            QVERIFY(m_codecInfo->supportedCodecs().contains(codec));
        } else {
            QVERIFY(!m_codecInfo->supportedCodecs().contains(codec));
        }
    }
}

void TestVideoCodecSupportInfo::codecName() {
    QCOMPARE(m_codecInfo->codecName(CV_FOURCC('I', 'Y', 'U', 'V')), QString("Raw Video"));
    QCOMPARE(m_codecInfo->codecName(CV_FOURCC('F', 'F', 'V', '1')), QString("FFV1 Lossless Video"));
    QCOMPARE(m_codecInfo->codecName(CV_FOURCC('L', 'A', 'G', 'S')), QString("Lagarith Lossless Video"));

    // case: non-existing codec
    QCOMPARE(m_codecInfo->codecName(CV_FOURCC('!', '!', '!', '!')), QString(""));
}

void TestVideoCodecSupportInfo::toFromFourcc() {
    QCOMPARE(m_codecInfo->stringToFourcc("IYUV"), CV_FOURCC('I', 'Y', 'U', 'V'));
    QCOMPARE(m_codecInfo->stringToFourcc("FFV1"), CV_FOURCC('F', 'F', 'V', '1'));
    QCOMPARE(m_codecInfo->stringToFourcc("LAGS"), CV_FOURCC('L', 'A', 'G', 'S'));
    QCOMPARE(m_codecInfo->stringToFourcc("1"), 0);

    QCOMPARE(m_codecInfo->fourccToString(CV_FOURCC('I', 'Y', 'U', 'V')), QString("IYUV"));
    QCOMPARE(m_codecInfo->fourccToString(CV_FOURCC('F', 'F', 'V', '1')), QString("FFV1"));
    QCOMPARE(m_codecInfo->fourccToString(CV_FOURCC('L', 'A', 'G', 'S')), QString("LAGS"));
    //QCOMPARE(VideoCodecSupportInfo::fourccToString(0), QString(""));

    QCOMPARE(m_codecInfo->fourccToEncoderString(CV_FOURCC('I', 'Y', 'U', 'V')), QString("rawvideo"));
    QCOMPARE(m_codecInfo->fourccToEncoderString(CV_FOURCC('F', 'F', 'V', '1')), QString("ffv1"));
    QCOMPARE(m_codecInfo->fourccToEncoderString(CV_FOURCC('L', 'A', 'G', 'S')), QString("lagarith"));
}

void TestVideoCodecSupportInfo::rawVideoCodecStr() {
    QCOMPARE(m_codecInfo->rawVideoCodecStr(), QString("IYUV"));
}

void TestVideoCodecSupportInfo::removeSupport() {
    QListIterator<int> codecIt(m_expectedCodecs.keys());

    while (codecIt.hasNext()) {
        int codec = codecIt.next();
        bool otherSupport = false;
        if (m_codecInfo->isEncoderSupported(codec)) {
            if (m_codecInfo->isOpencvSupported(codec)) {
                otherSupport = true;
            }
            m_codecInfo->removeSupport(codec, VideoCodecSupportInfo::External);
            QVERIFY(!m_codecInfo->isEncoderSupported(codec));
            if (otherSupport) {
                QVERIFY(m_codecInfo->isOpencvSupported(codec));
            }
        }
        if (m_codecInfo->isOpencvSupported(codec)) {
            m_codecInfo->removeSupport(codec, VideoCodecSupportInfo::OpenCv);
            QVERIFY(!m_codecInfo->isOpencvSupported(codec));
        }
        QVERIFY(!m_codecInfo->supportedCodecs().contains(codec));
    }
    QVERIFY(m_codecInfo->supportedCodecs().isEmpty());
}


QTEST_MAIN(TestVideoCodecSupportInfo)

#include "testVideoCodecSupportInfo.moc"
