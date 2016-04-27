#include "config.h"

Config::Config(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings("UFOID", "Detector");

    m_defaultResultDocumentDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/UFO ID";

#if defined (Q_OS_WIN)
    m_videoEncoderLocation = QCoreApplication::applicationDirPath()+"/ffmpeg.exe";
    /// @todo change data directory to something else than app directory also in Windows, because using writable files
    m_defaultDetectionDataDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/UFO ID";

#elif defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
    /// @todo use which(1) to find if ffmpeg and avconv are present. Prefer avconv at least in *ubuntu.
    m_videoEncoderLocation = "/usr/bin/avconv";
    // result DB should probably be somewhere in /var/, but detectionArea.xml should be somewhere under home
    m_defaultDetectionDataDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) +"/.UFO ID";
#endif

    m_defaultDetectionAreaFileName = m_defaultDetectionDataDir + "/detectionArea.xml";
    m_defaultBirdClassifierFileName = QCoreApplication::applicationDirPath() + "/cascade.xml";
    m_defaultResultDataFileName = m_defaultDetectionDataDir + "/logs.xml";

    m_defaultResultVideoDir = m_defaultResultDocumentDir + "/Videos";
    m_defaultResultImageDir = m_defaultResultDocumentDir + "/Images";
}

QString Config::videoEncoderLocation() {
    return m_videoEncoderLocation;
}

QString Config::detectionAreaFile() {
    return m_settings->value("detectionareafile", m_defaultDetectionAreaFileName).toString();
}

QString Config::birdClassifierTrainingFile() {
    return m_defaultBirdClassifierFileName;
}

QString Config::resultDataFile() {
    return m_settings->value("resultdatafile", m_defaultResultDataFileName).toString();
}

QString Config::resultVideoDir() {
    return m_settings->value("videofilepath", m_defaultResultVideoDir).toString();
}

QString Config::resultImageDir() {
    return m_settings->value("imagespath", m_defaultResultImageDir).toString();
}

void Config::setDetectionAreaFile(QString fileName) {
    m_settings->setValue("detectionareafile", QVariant(fileName));
}

void Config::setResultVideoDir(QString dirName) {
    m_settings->setValue("videofilepath", QVariant(dirName));
}

void Config::setResultImageDir(QString dirName) {
    m_settings->setValue("imagespath", QVariant(dirName));
}

void Config::resetDetectionAreaFile() {
    setDetectionAreaFile(m_defaultDetectionAreaFileName);
}

