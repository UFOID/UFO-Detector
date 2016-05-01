
#include "camerainfo.h"
#include <QString>
#include <QtTest>

class TestCameraInfo : public QObject
{
    Q_OBJECT

public:
    TestCameraInfo();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void queryResolutions();

private:
    CameraInfo* m_cameraInfo;
};

TestCameraInfo::TestCameraInfo()
{
}

void TestCameraInfo::initTestCase()
{
    m_cameraInfo = new CameraInfo(0);
}

void TestCameraInfo::cleanupTestCase()
{
    delete m_cameraInfo;
    m_cameraInfo = NULL;
}

void TestCameraInfo::queryResolutions()
{
    QVERIFY(m_cameraInfo->queryResolutions());
    QVERIFY(!m_cameraInfo->m_availableResolutions.isEmpty());
}

QTEST_APPLESS_MAIN(TestCameraInfo)

#include "testcamerainfo.moc"
