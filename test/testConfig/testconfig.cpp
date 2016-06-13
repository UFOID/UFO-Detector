#include "config.h"
#include <QString>
#include <QtTest>

class TestConfig : public QObject
{
    Q_OBJECT

public:
    TestConfig();

private Q_SLOTS:
    void init();
    void cleanup();
    void defaultValues();
    void motionThreshold();
    void videoCodecSupportInfo();

private:
    Config* m_config;
};

TestConfig::TestConfig() {
}

void TestConfig::init() {
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, QDir::currentPath());
    m_config = new Config();
    QVERIFY(NULL != m_config->m_settings);
    QVERIFY(QDir::currentPath() + "/UFOID/Detector.conf" == m_config->m_settings->fileName());
    QFile settingsFile(m_config->m_settings->fileName());
    if (settingsFile.exists()) {
        QVERIFY(settingsFile.remove());
    }
}

void TestConfig::cleanup() {
    QFile settingsFile(m_config->m_settings->fileName());
    if (settingsFile.exists()) {
        QVERIFY(settingsFile.remove());
    }
    delete m_config;
    m_config = NULL;
}

void TestConfig::defaultValues() {
    QVERIFY(m_config->applicationVersion() == "");
    QVERIFY(m_config->checkApplicationUpdates() == true);

    QVERIFY(m_config->cameraIndex() == 0);
    QVERIFY(m_config->cameraWidth() == 640);
    QVERIFY(m_config->cameraHeight() == 480);
    QVERIFY(m_config->checkCameraAspectRatio() == true);

    //QVERIFY(m_config->detectionAreaFile());
    QVERIFY(m_config->detectionAreaSize() == 0);

    QVERIFY(m_config->noiseFilterPixelSize() == 2);
    QVERIFY(m_config->motionThreshold() == 10);
    QVERIFY(m_config->minPositiveDetections() == 2);
    //QVERIFY(m_config->birdClassifierTrainingFile());

    //QVERIFY(m_config->resultDataFile());
    //QVERIFY(m_config->resultVideoDir());
    QVERIFY(m_config->resultVideoCodec() == 1);
    QVERIFY(m_config->resultVideoWithObjectRectangles() == false);
    //QVERIFY(m_config->videoEncoderLocation());
    //QVERIFY(m_config->resultImageDir());
    QVERIFY(m_config->saveResultImages() == false);

    QVERIFY(m_config->userTokenAtUfoId() == "");
}

void TestConfig::motionThreshold() {
    QFile settingsFile;
    QVERIFY("motionThreshold" == m_config->m_settingKeys[Config::MotionThreshold]);
    QVERIFY(!m_config->m_settings->contains(m_config->m_settingKeys[Config::MotionThreshold]));
    QVERIFY(10 == m_config->motionThreshold());
    QVERIFY(!m_config->m_settings->contains(m_config->m_settingKeys[Config::MotionThreshold]));
    m_config->setMotionThreshold(20);
    QVERIFY(m_config->m_settings->contains(m_config->m_settingKeys[Config::MotionThreshold]));
    QVERIFY(20 == m_config->motionThreshold());
    settingsFile.setFileName(m_config->m_settings->fileName());
    QVERIFY(settingsFile.exists());
}

void TestConfig::videoCodecSupportInfo() {
    QVERIFY(m_config->videoCodecSupportInfo() != NULL);
    QVERIFY(m_config->videoCodecSupportInfo()->isInitialized());
}

QTEST_MAIN(TestConfig)

#include "testconfig.moc"
