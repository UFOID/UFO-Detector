
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

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void queryResolutions();
    void compareResolutionsWidthFirst();
    void testListSorting();

private:
    CameraInfo* m_cameraInfo;
};

TestCameraInfo::TestCameraInfo()
{
    qDebug() << "NOTE: THIS UNIT TEST REQUIRES A WEB CAMERA";
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
    //QVERIFY(m_cameraInfo->queryResolutions());    // this is already done in constructor
    QVERIFY(!m_cameraInfo->m_availableResolutions.isEmpty());
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

QTEST_APPLESS_MAIN(TestCameraInfo)

#include "testcamerainfo.moc"
