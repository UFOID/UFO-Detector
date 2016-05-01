#include "camerainfo.h"

CameraInfo::CameraInfo(int cameraIndex, QObject *parent) : QObject(parent)
{
    m_cameraIndex = cameraIndex;
    /// @todo add common resolutions from https://en.wikipedia.org/wiki/List_of_common_resolutions
    m_commonResolutions << QSize(16, 16);
    m_commonResolutions << QSize(160, 120);
    m_commonResolutions << QSize(320, 200);
    m_commonResolutions << QSize(320, 240);
    m_commonResolutions << QSize(352, 288);
    m_commonResolutions << QSize(640, 480);
    m_commonResolutions << QSize(800, 600);
    m_commonResolutions << QSize(1024, 768);
}

QList<QSize> CameraInfo::availableResolutions() {
    queryResolutions();
    return m_availableResolutions;
}

bool CameraInfo::queryResolutions() {
    int width = 0;
    int height = 0;
    QSize testRes;
    QSize smallestResolution;
    QSize largestResolution;
    int smallestIndex;
    int largestIndex;
    if (!m_webCamera.open(m_cameraIndex)) {
        qDebug() << "Couldn't connect to web camera" << m_cameraIndex;
        return false;
    }

    qDebug() << "Querying web camera for supported resolutions";

    // find smallest supported resolution
    testRes = m_commonResolutions.at(0);
    m_webCamera.set(CV_CAP_PROP_FRAME_WIDTH, testRes.width());
    m_webCamera.set(CV_CAP_PROP_FRAME_HEIGHT, testRes.height());
    width = m_webCamera.get(CV_CAP_PROP_FRAME_WIDTH);
    height = m_webCamera.get(CV_CAP_PROP_FRAME_HEIGHT);
    smallestResolution.setWidth(width);
    smallestResolution.setHeight(height);
    smallestIndex = m_commonResolutions.indexOf(smallestResolution);
    if (smallestIndex < 0) {
        /// @todo find out the nearest index at lower side if resolution not in list
        smallestIndex = 0;
    }
    qDebug() << "Smallest resolution is" << width << "x" << height << "at index" << smallestIndex;

    // find largest supported resolution
    // Actually this test can be done in query loop below: just wait until size is larger than the last found
    testRes = m_commonResolutions.at(m_commonResolutions.size() - 1);
    m_webCamera.set(CV_CAP_PROP_FRAME_WIDTH, testRes.width());
    m_webCamera.set(CV_CAP_PROP_FRAME_HEIGHT, testRes.height());
    width = m_webCamera.get(CV_CAP_PROP_FRAME_WIDTH);
    height = m_webCamera.get(CV_CAP_PROP_FRAME_HEIGHT);
    largestResolution.setWidth(width);
    largestResolution.setHeight(height);
    largestIndex = m_commonResolutions.indexOf(largestResolution);
    if (largestIndex < 0) {
        /// @todo find out the nearest index at higher side if resolution not in list
        largestIndex = m_commonResolutions.size() - 1;
    }
    qDebug() << "Largest resolution is" << width << "x" << height << "at index" << largestIndex;

    for (int i = smallestIndex; i < (largestIndex + 1); i++) {
        testRes = m_commonResolutions.at(i);
        m_webCamera.set(CV_CAP_PROP_FRAME_WIDTH, testRes.width());
        m_webCamera.set(CV_CAP_PROP_FRAME_HEIGHT, testRes.height());
        width = m_webCamera.get(CV_CAP_PROP_FRAME_WIDTH);
        height = m_webCamera.get(CV_CAP_PROP_FRAME_HEIGHT);
        /// @todo if resolution is found which is not in common resolutions, add it to the available resolutions
        if ((testRes.width() == width) && (testRes.height() == height)) {
            qDebug() << testRes.width() << "x" << testRes.height() << "is supported";
            m_availableResolutions << testRes;
        } else {
            qDebug() << testRes.width() << "x" << testRes.height() << "is not supported";
        }
    }
    return true;
}

