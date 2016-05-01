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
    void testMotionThreshold();

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
    QVERIFY(settingsFile.remove());
    delete m_config;
    m_config = NULL;
}

void TestConfig::testMotionThreshold() {
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

QTEST_APPLESS_MAIN(TestConfig)

#include "testconfig.moc"
