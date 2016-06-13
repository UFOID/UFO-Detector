
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
    void init();
    void isSupportedMethods();

private:
    Config* m_config;
    VideoCodecSupportInfo* m_codecInfo;
    QString m_testFileName;
    bool localTestOpencvSupport(int fourcc);
    bool localTestEncoderSupport(QString encoderCodecStr);
};

TestVideoCodecSupportInfo::TestVideoCodecSupportInfo()
{
    m_config = NULL;
    m_codecInfo = NULL;
    m_testFileName = "dummy.avi";
}

void TestVideoCodecSupportInfo::initTestCase() {
    m_config = new Config();
    QVERIFY(NULL != m_config);
    m_codecInfo = new VideoCodecSupportInfo(m_config);
    QVERIFY(NULL != m_codecInfo);
}

void TestVideoCodecSupportInfo::cleanupTestCase() {
    m_codecInfo->deleteLater();
    m_config->deleteLater();
}

void TestVideoCodecSupportInfo::constructor() {
    QVERIFY(m_config == m_codecInfo->m_config);
}

bool TestVideoCodecSupportInfo::localTestOpencvSupport(int fourcc) {
    cv::VideoWriter writer;
    cv::Mat frame;
    bool supported = false;
    int writtenFourcc = 0;
    std::string testFileNameStd(m_testFileName.toLocal8Bit().data());

    // try opening & writing file
    if (!writer.open(testFileNameStd, fourcc, 25, cv::Size(m_config->cameraWidth(), m_config->cameraHeight()))) {
        return false;
    }
    if (!writer.isOpened()) {
        return false;
    }
    frame = cv::Mat(m_config->cameraHeight(), m_config->cameraWidth(), CV_8UC3);
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
    encoder.start(m_config->videoEncoderLocation(), args);
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


    startTime = QDateTime::currentMSecsSinceEpoch();
    QCOMPARE(m_codecInfo->testEncoderSupport(rawFourcc), rawSupported);
    QCOMPARE(m_codecInfo->testEncoderSupport(ffv1Fourcc), ffv1Supported);
    QCOMPARE(m_codecInfo->testEncoderSupport(lagarithFourcc), lagarithSupported);
    endTime = QDateTime::currentMSecsSinceEpoch();
    difference = endTime - startTime;
    qDebug() << "Checking encoder support status by VideoCodecSupportInfo took" << difference << "ms";

    // These apply to basic/non-PPA OpenCV2 packages in Ubuntu 14.04.4 (Trusty)
    //QVERIFY(rawFourcc);
    //QVERIFY(ffv1Supported);
    //QVERIFY(!lagarithSupported);
}

void TestVideoCodecSupportInfo::init() {
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
            encoderList.append(VideoCodecSupportInfo::Encoder);
            codecSupport.insert(codec, encoderList);
            QVERIFY(codecSupport.value(codec).contains(VideoCodecSupportInfo::Encoder));
        }
    }

    m_codecInfo->init();

    // check results
    codecIt.toFront();
    while (codecIt.hasNext()) {
        codec = codecIt.next();
        bool expectedOpencvSupported = codecSupport.value(codec).contains(VideoCodecSupportInfo::OpenCv);
        bool actualOpencvSupported = m_codecInfo->m_codecSupport.value(codec).contains(VideoCodecSupportInfo::OpenCv);
        QCOMPARE(actualOpencvSupported, expectedOpencvSupported);

        bool expectedEncoderSupported = codecSupport.value(codec).contains(VideoCodecSupportInfo::Encoder);
        bool actualEncoderSupported = m_codecInfo->m_codecSupport.value(codec).contains(VideoCodecSupportInfo::Encoder);
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
            encoderList.append(VideoCodecSupportInfo::Encoder);
            codecSupport.insert(codec, encoderList);
            QVERIFY(codecSupport.value(codec).contains(VideoCodecSupportInfo::Encoder));
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

        bool expectedEncoderSupported = codecSupport.value(codec).contains(VideoCodecSupportInfo::Encoder);
        bool actualEncoderSupported = m_codecInfo->isEncoderSupported(codec);
        QCOMPARE(actualEncoderSupported, expectedEncoderSupported);
    }
}

QTEST_MAIN(TestVideoCodecSupportInfo)

#include "testVideoCodecSupportInfo.moc"
