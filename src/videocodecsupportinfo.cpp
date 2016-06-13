#include "videocodecsupportinfo.h"

VideoCodecSupportInfo::VideoCodecSupportInfo(QString externalVideoEncoderLocation, QObject* parent)
    : QObject(parent)
{
    m_videoEncoderLocation = externalVideoEncoderLocation;
    m_testFileName = "dummy_Wa8F7bVL3lmF4ngfD0894u32Nd.avi";
    m_isInitialized = false;

    m_codecSupport.insert(toFourcc("IYUV"), QList<int>());
    m_codecSupport.insert(toFourcc("FFV1"), QList<int>());
    m_codecSupport.insert(toFourcc("LAGS"), QList<int>());

    m_fourccToEncoderStr.insert(toFourcc("IYUV"), "rawvideo");
    m_fourccToEncoderStr.insert(toFourcc("FFV1"), "ffv1");
    m_fourccToEncoderStr.insert(toFourcc("LAGS"), "lagarith");
}

bool VideoCodecSupportInfo::init() {
    if (m_isInitialized) {
        return false;
    }
    int codec = 0;
    QListIterator<int> codecIt(m_codecSupport.keys());
    QList<int> encoderList;
    QFile encoder(m_videoEncoderLocation);

    if (!encoder.exists()) {
        qDebug() << "Video encoder" << m_videoEncoderLocation << "doesn't exist";
        return false;
    }

    while (codecIt.hasNext()) {
        codec = codecIt.next();
        if (testOpencvSupport(codec)) {
            encoderList = m_codecSupport.value(codec);
            encoderList.append(VideoCodecSupportInfo::OpenCv);
            m_codecSupport.insert(codec, encoderList);
        }
        if (testEncoderSupport(codec)) {
            encoderList = m_codecSupport.value(codec);
            encoderList.append(VideoCodecSupportInfo::External);
            m_codecSupport.insert(codec, encoderList);
        }
    }
    m_isInitialized = true;
    return true;
}

bool VideoCodecSupportInfo::isInitialized() {
    return m_isInitialized;
}

// static
int VideoCodecSupportInfo::toFourcc(QString fourccStr) {
    char* str = fourccStr.toLocal8Bit().data();
    return CV_FOURCC(str[0], str[1], str[2], str[3]);
}

bool VideoCodecSupportInfo::isOpencvSupported(int fourcc) {
    return m_codecSupport.value(fourcc).contains(VideoCodecSupportInfo::OpenCv);
}

bool VideoCodecSupportInfo::isEncoderSupported(int fourcc) {
    return m_codecSupport.value(fourcc).contains(VideoCodecSupportInfo::External);
}

QList<int> VideoCodecSupportInfo::supportedCodecs() {
    QList<int> codecs;
    QListIterator<int> codecIt(m_codecSupport.keys());
    int codec = 0;

    while (codecIt.hasNext()) {
        codec = codecIt.next();
        if (!m_codecSupport.value(codec).isEmpty()) {
            codecs << codec;
        }
    }
    return codecs;
}

bool VideoCodecSupportInfo::testOpencvSupport(int fourcc) {
    cv::VideoWriter writer;
    cv::Mat frame;
    bool supported = false;
    int writtenFourcc = 0;
    std::string testFileNameStd(m_testFileName.toLocal8Bit().data());

    // try opening & writing file
    if (!writer.open(testFileNameStd, fourcc, 25, cv::Size(640, 480))) {
        return false;
    }
    if (!writer.isOpened()) {
        return false;
    }
    frame = cv::Mat(480, 640, CV_8UC3);
    writer.write(frame);
    writer.release();

    // check result
    cv::VideoCapture reader;
    if (!reader.open(testFileNameStd)) {
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

bool VideoCodecSupportInfo::testEncoderSupport(int fourcc) {
    QProcess encoder;
    QStringList encoderOutput;
    QStringList args;
    QString encoderCodecStr = m_fourccToEncoderStr.value(fourcc);

    if (encoderCodecStr.isEmpty()) {
        return false;
    }

    args << "-codecs";
    encoder.start(m_videoEncoderLocation, args);
    encoder.waitForFinished();
    while (encoder.canReadLine()) {
        encoderOutput << encoder.readLine();
    }
    QStringListIterator listIt(encoderOutput);
    while (listIt.hasNext()) {
        QString line = listIt.next();
        // Find encoder name and verify it can encode videos.
        QRegExp regex("^.EV...\\ " + encoderCodecStr + ".*$");
        if (regex.exactMatch(line)) {
            return true;
        }
    }
    return false;
}

