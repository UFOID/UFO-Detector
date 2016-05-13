#include "config.h"

Config::Config(QObject *parent) : QObject(parent)
{
    m_settingKeys[Config::ApplicationVersion] = "applicationVersion";
    m_settingKeys[Config::CheckApplicationUpdates] = "checkApplicationUpdates";
    m_settingKeys[Config::CameraIndex] = "cameraIndex";
    m_settingKeys[Config::CameraWidth] = "cameraWidth";
    m_settingKeys[Config::CameraHeight] = "cameraHeight";
    m_settingKeys[Config::CheckCameraAspectRatio] = "checkCameraAspectRatio";
    m_settingKeys[Config::DetectionAreaFile] = "detectionAreaFile";
    m_settingKeys[Config::DetectionAreaSize] = "detectionAreaSize";
    m_settingKeys[Config::NoiseFilterPixelSize] = "noiseFilterPixelSize";
    m_settingKeys[Config::MotionThreshold] = "motionThreshold";
    m_settingKeys[Config::MinPositiveDetections] = "minPositiveDetections";
    m_settingKeys[Config::BirdClassifierTrainingFile] = "birdClassifierTrainingFile";
    m_settingKeys[Config::ResultDataFile] = "resultDataFile";
    m_settingKeys[Config::ResultVideoDir] = "resultVideoDir";
    m_settingKeys[Config::ResultVideoCodec] = "resultVideoCodec";
    m_settingKeys[Config::ResultVideoWithObjectRectangles] = "resultVideoWithObjectRectangles";
    m_settingKeys[Config::VideoEncoderLocation] = "videoEncoderLocation";
    m_settingKeys[Config::ResultImageDir] = "resultImageDir";
    m_settingKeys[Config::SaveResultImages] = "saveResultImages";
    m_settingKeys[Config::UserTokenAtUfoId] = "userTokenAtUfoId";

    m_settings = new QSettings("UFOID", "Detector");

    m_defaultApplicationVersion = "";
    m_defaultCheckApplicationUpdates = true;

    m_defaultCameraIndex = 0;
    m_defaultCameraWidth = 640;
    m_defaultCameraHeight = 480;
    m_defaultCheckCameraAspectRatio = true;

    m_defaultNoiseFilterPixelSize = 2;
    m_defaultMotionThreshold = 10;
    m_defaultMinPositiveDetections = 2;
    m_defaultBirdClassifierFileName = QCoreApplication::applicationDirPath() + "/cascade.xml";

#if defined (Q_OS_WIN)
    m_defaultVideoEncoderLocation = QCoreApplication::applicationDirPath()+"/ffmpeg.exe";
    m_defaultDetectionDataDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/UFO ID";

#elif defined (Q_OS_LINUX) || defined (Q_OS_UNIX)
    /// @todo use which(1) to find if ffmpeg and avconv are present. Prefer avconv at least in *ubuntu.
    m_defaultVideoEncoderLocation = "/usr/bin/avconv";
    // result DB should probably be somewhere in /var/, but detectionArea.xml should be somewhere under home
    m_defaultDetectionDataDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) +"/.UFO ID";
#endif

    m_defaultResultDataFileName = m_defaultDetectionDataDir + "/logs.xml";
    m_defaultResultDocumentDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+"/UFO ID";
    m_defaultDetectionAreaFileName = m_defaultDetectionDataDir + "/detectionArea.xml";

    m_defaultResultVideoDir = m_defaultResultDocumentDir + "/Videos";
    if ((QSysInfo::windowsVersion()==QSysInfo::WV_WINDOWS8) || (QSysInfo::windowsVersion()==QSysInfo::WV_WINDOWS8_1)) {
        m_defaultVideoCodec = 2;
    } else {
        m_defaultVideoCodec = 1;
    }
    m_defaultResultVideoWithRectangles = false;
    m_defaultResultImageDir = m_defaultResultDocumentDir + "/Images";
    m_defaultSaveResultImages = false;

    m_defaultUserTokenAtUfoId = "";
}

QString Config::applicationVersion() {
    return m_settings->value(m_settingKeys[Config::ApplicationVersion], m_defaultApplicationVersion).toString();
}

bool Config::checkApplicationUpdates() {
    return m_settings->value(m_settingKeys[Config::CheckApplicationUpdates], m_defaultCheckApplicationUpdates).toBool();
}

int Config::cameraIndex() {
    return m_settings->value(m_settingKeys[Config::CameraIndex], m_defaultCameraIndex).toInt();
}

int Config::cameraWidth() {
    return m_settings->value(m_settingKeys[Config::CameraWidth], m_defaultCameraWidth).toInt();
}

int Config::cameraHeight() {
    return m_settings->value(m_settingKeys[Config::CameraHeight], m_defaultCameraHeight).toInt();
}

bool Config::checkCameraAspectRatio() {
    return m_settings->value(m_settingKeys[Config::CheckCameraAspectRatio], m_defaultCheckCameraAspectRatio).toBool();
}

QString Config::detectionAreaFile() {
    return m_settings->value(m_settingKeys[Config::DetectionAreaFile], m_defaultDetectionAreaFileName).toString();
}

int Config::noiseFilterPixelSize() {
    return m_settings->value(m_settingKeys[Config::NoiseFilterPixelSize], m_defaultNoiseFilterPixelSize).toInt();
}

int Config::motionThreshold() {
    return m_settings->value(m_settingKeys[Config::MotionThreshold], m_defaultMotionThreshold).toInt();
}

int Config::minPositiveDetections() {
    return m_settings->value(m_settingKeys[Config::MinPositiveDetections], m_defaultMinPositiveDetections).toInt();
}

QString Config::birdClassifierTrainingFile() {
    return m_defaultBirdClassifierFileName;
}

QString Config::resultDataFile() {
    return m_settings->value(m_settingKeys[Config::ResultDataFile], m_defaultResultDataFileName).toString();
}

QString Config::resultVideoDir() {
    return m_settings->value(m_settingKeys[Config::ResultVideoDir], m_defaultResultVideoDir).toString();
}

int Config::resultVideoCodec() {
    return m_settings->value(m_settingKeys[Config::ResultVideoCodec], m_defaultVideoCodec).toInt();
}

bool Config::resultVideoWithObjectRectangles() {
    return m_settings->value(m_settingKeys[Config::ResultVideoWithObjectRectangles], m_defaultResultVideoWithRectangles).toBool();
}

QString Config::videoEncoderLocation() {
    return m_defaultVideoEncoderLocation;
}

QString Config::resultImageDir() {
    return m_settings->value(m_settingKeys[Config::ResultImageDir], m_defaultResultImageDir).toString();
}

bool Config::saveResultImages() {
    return m_settings->value(m_settingKeys[Config::SaveResultImages], m_defaultSaveResultImages).toBool();
}

int Config::detectionAreaSize() {
    return m_settings->value(m_settingKeys[Config::MotionThreshold], m_defaultDetectionAreaSize).toInt();
}

QString Config::userTokenAtUfoId() {
    return m_settings->value(m_settingKeys[Config::UserTokenAtUfoId], m_defaultUserTokenAtUfoId).toString();
}

void Config::setApplicationVersion(QString version) {
    m_settings->setValue(m_settingKeys[Config::ApplicationVersion], QVariant(version));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setCameraIndex(int index) {
    m_settings->setValue(m_settingKeys[Config::CameraIndex], QVariant(index));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setCameraWidth(int width) {
    m_settings->setValue(m_settingKeys[Config::CameraWidth], QVariant(width));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setCameraHeight(int height) {
    m_settings->setValue(m_settingKeys[Config::CameraHeight], QVariant(height));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setDetectionAreaFile(QString fileName) {
    m_settings->setValue(m_settingKeys[Config::DetectionAreaFile], QVariant(fileName));
    m_settings->sync();
    emit settingsChanged();
}

void Config::resetDetectionAreaFile() {
    setDetectionAreaFile(m_defaultDetectionAreaFileName);
    m_settings->sync();
    emit settingsChanged();
}

void Config::setDetectionAreaSize(int areaSize) {
    m_settings->setValue(m_settingKeys[Config::DetectionAreaSize], QVariant(areaSize));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setNoiseFilterPixelSize(int size) {
    m_settings->setValue(m_settingKeys[Config::NoiseFilterPixelSize], QVariant(size));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setMotionThreshold(int threshold) {
    m_settings->setValue(m_settingKeys[Config::MotionThreshold], QVariant(threshold));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setMinPositiveDetections(int detectionCount) {
    m_settings->setValue(m_settingKeys[Config::MinPositiveDetections], QVariant(detectionCount));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setResultVideoDir(QString dirName) {
    m_settings->setValue(m_settingKeys[Config::ResultVideoDir], QVariant(dirName));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setResultVideoCodec(int codec) {
    m_settings->setValue(m_settingKeys[Config::ResultVideoCodec], QVariant(codec));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setResultVideoWithObjectRectangles(bool drawRectangles) {
    m_settings->setValue(m_settingKeys[Config::ResultVideoWithObjectRectangles], QVariant(drawRectangles));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setResultImageDir(QString dirName) {
    m_settings->setValue(m_settingKeys[Config::ResultImageDir], QVariant(dirName));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setSaveResultImages(bool save) {
    m_settings->setValue(m_settingKeys[Config::SaveResultImages], QVariant(save));
    m_settings->sync();
    emit settingsChanged();
}

void Config::setUserTokenAtUfoId(QString token) {
    m_settings->setValue(m_settingKeys[Config::UserTokenAtUfoId], QVariant(token));
    m_settings->sync();
    emit settingsChanged();
}
