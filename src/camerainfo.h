#ifndef CAMERAINFO_H
#define CAMERAINFO_H

#include "opencv2/highgui/highgui.hpp"
#include <QObject>
#include <QList>
#include <QSize>
#include <QDebug>

/**
 * @brief Get information of web camera
 */
class CameraInfo : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief CameraInfo
     * @param cameraIndex index of camera as used by OpenCV
     * @param parent
     */
    explicit CameraInfo(int cameraIndex, QObject *parent = 0);

    /**
     * @brief get list of available resolutions
     * @return list of available resolutions
     */
    QList<QSize> availableResolutions();

#ifndef _UNIT_TEST_
private:
#endif
    int m_cameraIndex;  ///< index of camera as used by OpenCV
    cv::VideoCapture m_webCamera;
    QList<QSize> m_commonResolutions;   ///< common resolutions from Wikipedia
    QList<QSize> m_availableResolutions;    ///< results of resolution querying

    /**
     * @brief queryResolutions query available resolutions from web camera
     * @return
     */
    bool queryResolutions();

signals:

public slots:

};

#endif // CAMERAINFO_H
