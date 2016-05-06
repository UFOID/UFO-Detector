#ifndef CAMERAINFO_H
#define CAMERAINFO_H

#include "opencv2/highgui/highgui.hpp"
#include <QObject>
#include <QList>
#include <list>     // for std::list::sort
#include <algorithm>
#include <QSize>
#include <QDebug>

/**
 * @brief Get information of web camera
 * NOTE: web camera must not be reserved when creating this object, so create
 * this before using camera
 */
class CameraInfo : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief CameraInfo
     * @param cameraIndex index of camera as used by OpenCV
     * @param parent
     * @param openCvBackend backend for OpenCV VideoCapture object which is used by this class. Refer to VideoCapture::VideoCapture(int) for details.
     */
    explicit CameraInfo(int cameraIndex, QObject *parent = 0, int openCvBackend = CV_CAP_ANY);

    /**
     * @brief init initialize
     * @return true if initialization was successful, false otherwise
     */
    bool init();

    /**
     * @brief availableResolutions get list of available resolutions
     * @return list of available resolutions
     */
    QList<QSize> availableResolutions();

    /**
     * @brief compareResolutionsWidthFirst compare resolutions giving precedence to width
     * This method is suitable to be given to std::sort as compare function
     * @param res1
     * @param res2
     * @return true if res1 < res2 (precedence is on width)
     */
    static bool compareResolutionsWidthFirst(QSize res1, QSize res2);

#ifndef _UNIT_TEST_
private:
#endif
    int m_cameraIndex;  ///< index of camera as used by OpenCV
    int m_cameraBackend;    ///< camera backend as used by OpenCV
    cv::VideoCapture* m_webCamera;
    QList<QSize> m_commonResolutions;   ///< common resolutions from Wikipedia
    QList<QSize> m_availableResolutions;    ///< results of resolution querying

    /**
     * @brief queryResolutions query available resolutions from web camera
     * @return true if querying succeeded, false if it failed
     */
    bool queryResolutions();

signals:
    /**
     * @brief queryProgressChanged emitted when querying progresses
     * @param percent percent of query ready
     */
    void queryProgressChanged(int percent);

public slots:

};

#endif // CAMERAINFO_H
