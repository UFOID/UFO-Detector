
#include "camerainfo.h"
#include <QString>
#include <QtTest>

/**
 * @brief The TestCameraInfo class unit test for CameraInfo class
 * Drawback: needs a real web camera at the moment. If you know how to make
 * a mock object of cv::VideoCapture please tell it. :)
 */
class TestCameraInfo : public QObject
{
    Q_OBJECT

public:
    TestCameraInfo();

public slots:
    void onQueryProgressChanged(int progress);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void cameraInfoInit();  // "init" seems to be a reserved word in QTest so using other name
    void compareResolutionsWidthFirst();
    void testListSorting();
    void testQueryPerformance();

private:
    CameraInfo* m_cameraInfo;
    int m_lastQueryProgress;
};

TestCameraInfo::TestCameraInfo()
{
    qDebug() << "NOTE: THIS UNIT TEST REQUIRES A WEB CAMERA";
    m_cameraInfo = NULL;
    m_lastQueryProgress = 0;
}

void TestCameraInfo::onQueryProgressChanged(int progress) {
    m_lastQueryProgress = progress;
}

void TestCameraInfo::initTestCase()
{
    m_cameraInfo = new CameraInfo(0);
    QVERIFY(NULL != m_cameraInfo);
    QVERIFY(m_cameraInfo->m_commonResolutions.size() > 0);
    QVERIFY(m_cameraInfo->m_availableResolutions.isEmpty());
    connect(m_cameraInfo, SIGNAL(queryProgressChanged(int)), this, SLOT(onQueryProgressChanged(int)));
}

void TestCameraInfo::cleanupTestCase()
{
    if (m_cameraInfo) {
        m_cameraInfo->deleteLater();
    }
}

void TestCameraInfo::cameraInfoInit()
{
    QVERIFY(m_cameraInfo->m_availableResolutions.isEmpty());
    QVERIFY(0 == m_lastQueryProgress);
    QVERIFY(m_cameraInfo->init());
    QVERIFY(!m_cameraInfo->m_availableResolutions.isEmpty());
    QVERIFY(100 == m_lastQueryProgress);

    QListIterator<QSize> listIt(m_cameraInfo->m_availableResolutions);
    qDebug() << "Resolutions found in unit test:";
    while (listIt.hasNext()) {
        QSize resolution = (QSize)listIt.next();
        qDebug() << resolution.width() << "x" << resolution.height();
    }
}

void TestCameraInfo::compareResolutionsWidthFirst() {
    QSize res1;
    QSize res2;

    // res1 < res2, both width and height
    res1.setWidth(10);
    res1.setHeight(10);
    res2.setWidth(20);
    res2.setHeight(20);
    QVERIFY(CameraInfo::compareResolutionsWidthFirst(res1, res2));

    // res1 < res2, only width, height equal
    res1.setWidth(10);
    res1.setHeight(10);
    res2.setWidth(20);
    res2.setHeight(10);
    QVERIFY(CameraInfo::compareResolutionsWidthFirst(res1, res2));

    // res1 < res2, only width, height less in res2
    res1.setWidth(10);
    res1.setHeight(10);
    res2.setWidth(20);
    res2.setHeight(0);
    QVERIFY(CameraInfo::compareResolutionsWidthFirst(res1, res2));

    // res1 == res2, both width and height
    res1.setWidth(10);
    res1.setHeight(10);
    res2.setWidth(10);
    res2.setHeight(10);
    QVERIFY(!CameraInfo::compareResolutionsWidthFirst(res1, res2));

    // res1 == res2, width equal, height larger in res2
    res1.setWidth(10);
    res1.setHeight(10);
    res2.setWidth(10);
    res2.setHeight(20);
    QVERIFY(CameraInfo::compareResolutionsWidthFirst(res1, res2));

    // res1 == res2, width equal, height smaller in res2
    res1.setWidth(10);
    res1.setHeight(10);
    res2.setWidth(10);
    res2.setHeight(0);
    QVERIFY(!CameraInfo::compareResolutionsWidthFirst(res1, res2));

    // res1 > res2, res2 width is smaller, otherwise equal
    res1.setWidth(20);
    res1.setHeight(10);
    res2.setWidth(10);
    res2.setHeight(10);
    QVERIFY(!CameraInfo::compareResolutionsWidthFirst(res1, res2));

    // res1 > res2, res2 width and height are smaller
    res1.setWidth(20);
    res1.setHeight(20);
    res2.setWidth(10);
    res2.setHeight(10);
    QVERIFY(!CameraInfo::compareResolutionsWidthFirst(res1, res2));

    // res1 > res2, res2 height is smaller, width is same
    res1.setWidth(10);
    res1.setHeight(20);
    res2.setWidth(10);
    res2.setHeight(10);
    QVERIFY(!CameraInfo::compareResolutionsWidthFirst(res1, res2));
}

void TestCameraInfo::testListSorting() {
    QList<QSize> testList;
    testList << QSize(50, 10);
    testList << QSize(20, 40);
    testList << QSize(10, 50);
    testList << QSize(20, 20);
    testList << QSize(20, 30);

    std::sort(testList.begin(), testList.end(), CameraInfo::compareResolutionsWidthFirst);
    QVERIFY(testList.at(0) == QSize(10, 50));
    QVERIFY(testList.at(1) == QSize(20, 20));
    QVERIFY(testList.at(2) == QSize(20, 30));
    QVERIFY(testList.at(3) == QSize(20, 40));
    QVERIFY(testList.at(4) == QSize(50, 10));
}

void TestCameraInfo::testQueryPerformance() {
    QList<int> backendList;
    QList<QString> backendStringList;
    QList<int> durationMsecList;
    QTime duration;
#ifdef Q_OS_WINDOWS
    backendList << CV_CAP_MSMF;
    backendList << CV_CAP_QT;
    backendList << CV_CAP_WINRT;
    backendStringList << "CV_CAP_MSMF";
    backendStringList << "CV_CAP_QT";
    backendStringList << "CV_CAP_WINRT";
#else
    backendList << CV_CAP_ANY;
    backendStringList << "CV_CAP_ANY";
#endif
    QListIterator<int> backendIt(backendList);
    while (backendIt.hasNext()) {
        int backend = backendIt.next();
        delete m_cameraInfo;
        m_cameraInfo = new CameraInfo(0, 0, backend);
        duration.start();
        m_cameraInfo->init();
        durationMsecList << duration.elapsed();
    }
    backendIt.toFront();
    QListIterator<int> durationIt(durationMsecList);
    QListIterator<QString> backendStringIt(backendStringList);
    while (backendStringIt.hasNext()) {
        QString backendStr = backendStringIt.next();
        int msec = durationIt.next();
        qDebug() << "VideoCapture backend" << backendStr << "query duration" << (double)msec/1000 << "s";
    }
}

QTEST_APPLESS_MAIN(TestCameraInfo)

#include "testcamerainfo.moc"
