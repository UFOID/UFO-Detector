/*
 * UFO Detector | www.UFOID.net
 *
 * Copyright (C) 2016 UFOID
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
     *
     * NOTE: web camera must not be reserved when calling this method so release
     * the camera first
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init();

    /**
     * @brief isInitialized check whether this instance of CameraInfo is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized();

    /**
     * @brief availableResolutions get list of available resolutions
     * @return list of available resolutions
     */
    QList<QSize> availableResolutions();

    /**
     * @brief knownAspectRatios list of known web camera aspect ratios
     * @return
     */
    QList<int> knownAspectRatios();

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
    QList<int> m_knownAspectRatios;          ///< aspect ratios (*10,000) for common and available resolutions
    QList<QSize> m_availableResolutions;    ///< results of resolution querying
    bool m_initialized;

    /**
     * @brief queryResolutions query available resolutions from web camera
     * @return true if querying succeeded, false if it failed
     */
    bool queryResolutions();

    /**
     * @brief updateAspectRatios update aspect ratio list based on common and available resolutions
     */
    void updateAspectRatios();

signals:
    /**
     * @brief queryProgressChanged emitted when querying progresses
     * @param percent percent of query ready
     */
    void queryProgressChanged(int percent);

public slots:

};

#endif // CAMERAINFO_H
