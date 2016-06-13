#include "videocodecsupportinfo.h"

VideoCodecSupportInfo::VideoCodecSupportInfo(Config* config, QObject* parent)
    : QObject(parent)
{
    m_config = config;
    m_testFileName = "dummy_Wa8F7bVL3lmF4ngfD0894u32Nd.avi";

    m_codecSupport.insert(toFourcc("IYUV"), QList<int>());
    m_codecSupport.insert(toFourcc("FFV1"), QList<int>());
    m_codecSupport.insert(toFourcc("LAGS"), QList<int>());

    m_fourccToEncoderStr.insert(toFourcc("IYUV"), "rawvideo");
    m_fourccToEncoderStr.insert(toFourcc("FFV1"), "ffv1");
    m_fourccToEncoderStr.insert(toFourcc("LAGS"), "lagarith");
}

void VideoCodecSupportInfo::init() {
}

// static
int VideoCodecSupportInfo::toFourcc(QString fourccStr) {
    char* str = fourccStr.toLocal8Bit().data();
    return CV_FOURCC(str[0], str[1], str[2], str[3]);
}

bool VideoCodecSupportInfo::isOpencvSupported(int fourcc) {
    Q_UNUSED(fourcc);
    return false;
}

bool VideoCodecSupportInfo::isOpencvSupported(QString fourccStr) {
    Q_UNUSED(fourccStr);
    return false;
}

bool VideoCodecSupportInfo::isEncoderSupported(int fourcc) {
    Q_UNUSED(fourcc);
    return false;
}

bool VideoCodecSupportInfo::isEncoderSupported(QString fourccStr) {
    Q_UNUSED(fourccStr);
    return false;
}

bool VideoCodecSupportInfo::testOpencvSupport(int fourcc) {
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
    encoder.start(m_config->videoEncoderLocation(), args);
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

