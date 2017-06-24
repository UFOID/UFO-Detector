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

#include "camerainfo.h"

CameraInfo::CameraInfo(int cameraIndex, QObject *parent, int openCvBackend) : QObject(parent)
{
    m_cameraIndex = cameraIndex;
    m_cameraBackend = openCvBackend;

    // Source: https://en.wikipedia.org/wiki/List_of_common_resolutions (referenced 2016-05-02)
    // Used sections Common display formats, Video conferencing standards, Post-production

#ifndef Q_OS_WINDOWS
    m_commonResolutions << QSize(32, 32);
    m_commonResolutions << QSize(40, 30);
    m_commonResolutions << QSize(42, 11);
    m_commonResolutions << QSize(42, 32);
    m_commonResolutions << QSize(48, 32);
    m_commonResolutions << QSize(60, 40);
    m_commonResolutions << QSize(64, 64);
    m_commonResolutions << QSize(72, 64);
    m_commonResolutions << QSize(75, 64);
    m_commonResolutions << QSize(84, 48);
    m_commonResolutions << QSize(96, 64);
    m_commonResolutions << QSize(96, 65);
    m_commonResolutions << QSize(96, 96);
    m_commonResolutions << QSize(102, 64);
    m_commonResolutions << QSize(128, 36);
    m_commonResolutions << QSize(128, 48);
    m_commonResolutions << QSize(128, 96);
    m_commonResolutions << QSize(128, 128);
    m_commonResolutions << QSize(140, 192);
    m_commonResolutions << QSize(144, 168);
    m_commonResolutions << QSize(150, 40);
    m_commonResolutions << QSize(160, 102);
    m_commonResolutions << QSize(160, 120);	// QQVGA
    m_commonResolutions << QSize(160, 144);
    m_commonResolutions << QSize(160, 152);
    m_commonResolutions << QSize(160, 160);
    m_commonResolutions << QSize(160, 200);
    m_commonResolutions << QSize(160, 256);
    m_commonResolutions << QSize(176, 144);
    m_commonResolutions << QSize(208, 176);
    m_commonResolutions << QSize(208, 208);
    m_commonResolutions << QSize(220, 176);
    m_commonResolutions << QSize(224, 144);
    m_commonResolutions << QSize(240, 64);
    m_commonResolutions << QSize(240, 160);
    m_commonResolutions << QSize(240, 240);
    m_commonResolutions << QSize(256, 192);
    m_commonResolutions << QSize(256, 256);
    m_commonResolutions << QSize(272, 340);
    m_commonResolutions << QSize(280, 192);
    m_commonResolutions << QSize(312, 390);
    m_commonResolutions << QSize(320, 192);
    m_commonResolutions << QSize(320, 200);
    m_commonResolutions << QSize(320, 208);
    m_commonResolutions << QSize(320, 224);
    m_commonResolutions << QSize(320, 240);	// QVGA
    m_commonResolutions << QSize(320, 256);
    m_commonResolutions << QSize(320, 320);
    m_commonResolutions << QSize(352, 288);	// CIF
    m_commonResolutions << QSize(376, 240);
    m_commonResolutions << QSize(384, 288);	// CIF
    m_commonResolutions << QSize(400, 240);	// WQVGA
    m_commonResolutions << QSize(400, 270);
    m_commonResolutions << QSize(400, 300);
    m_commonResolutions << QSize(416, 352);
    m_commonResolutions << QSize(432, 128);
    m_commonResolutions << QSize(432, 240);	// WQVGA
    m_commonResolutions << QSize(480, 234);	// WQVGA
    m_commonResolutions << QSize(480, 250);
    m_commonResolutions << QSize(480, 272);
    m_commonResolutions << QSize(480, 320);	// HVGA
    m_commonResolutions << QSize(480, 500);
    m_commonResolutions << QSize(512, 256);
    m_commonResolutions << QSize(512, 342);
    m_commonResolutions << QSize(512, 384);
    m_commonResolutions << QSize(560, 192);
    m_commonResolutions << QSize(600, 480);
    m_commonResolutions << QSize(640, 200);
    m_commonResolutions << QSize(640, 240);
    m_commonResolutions << QSize(640, 256);	// HVGA
    m_commonResolutions << QSize(640, 320);
    m_commonResolutions << QSize(640, 350);	// EGA
    m_commonResolutions << QSize(640, 360);
    m_commonResolutions << QSize(640, 400);
    m_commonResolutions << QSize(640, 480);	// VGA
    m_commonResolutions << QSize(640, 512);
    m_commonResolutions << QSize(704, 576);	// 4CIF
    m_commonResolutions << QSize(720, 348);
    m_commonResolutions << QSize(720, 350);
    m_commonResolutions << QSize(720, 364);
    m_commonResolutions << QSize(720, 480);	// DV NTSC
    m_commonResolutions << QSize(720, 486);	// D1 NTSC
    m_commonResolutions << QSize(720, 576);	// DV PAL
    m_commonResolutions << QSize(720, 576);	// D1 PAL
    m_commonResolutions << QSize(768, 480);	// WVGA
    m_commonResolutions << QSize(800, 240);
    m_commonResolutions << QSize(800, 352);
    m_commonResolutions << QSize(800, 480);	// WGA
    m_commonResolutions << QSize(800, 600);	// SVGA
    m_commonResolutions << QSize(832, 624);
    m_commonResolutions << QSize(848, 480);
    m_commonResolutions << QSize(854, 480);	// FWVGA
    m_commonResolutions << QSize(960, 540);
    m_commonResolutions << QSize(960, 544);
    m_commonResolutions << QSize(960, 640);
    m_commonResolutions << QSize(960, 720);
    m_commonResolutions << QSize(1024, 567);	// PAL 16:9
    m_commonResolutions << QSize(1024, 600);	// WSVGA
    m_commonResolutions << QSize(1024, 640);
    m_commonResolutions << QSize(1024, 768);	// XGA
    m_commonResolutions << QSize(1024, 800);
    m_commonResolutions << QSize(1024, 1024);
    m_commonResolutions << QSize(1120, 832);
    m_commonResolutions << QSize(1136, 640);
    m_commonResolutions << QSize(1152, 720);
    m_commonResolutions << QSize(1152, 768);
    m_commonResolutions << QSize(1152, 864);
    m_commonResolutions << QSize(1152, 900);
    m_commonResolutions << QSize(1280, 720);	// 720p
    m_commonResolutions << QSize(1280, 768);
    m_commonResolutions << QSize(1280, 800);
    m_commonResolutions << QSize(1280, 854);
    m_commonResolutions << QSize(1280, 960);
    m_commonResolutions << QSize(1280, 1024);
    m_commonResolutions << QSize(1280, 1080);
    m_commonResolutions << QSize(1334, 740);
    m_commonResolutions << QSize(1366, 768);	// HDTV 720p/1080i
    m_commonResolutions << QSize(1400, 1050);
    m_commonResolutions << QSize(1408, 1152);	// 16CIF
    m_commonResolutions << QSize(1440, 900);
    m_commonResolutions << QSize(1440, 960);
    m_commonResolutions << QSize(1440, 1024);
    m_commonResolutions << QSize(1440, 1080);
    m_commonResolutions << QSize(1600, 768);
    m_commonResolutions << QSize(1600, 900);	// 900p
    m_commonResolutions << QSize(1600, 1024);
    m_commonResolutions << QSize(1600, 1200);	// UXGA
    m_commonResolutions << QSize(1600, 1280);
    m_commonResolutions << QSize(1680, 1050);	// WSXGA+
    m_commonResolutions << QSize(1792, 1344);
    m_commonResolutions << QSize(1800, 1440);
    m_commonResolutions << QSize(1856, 1392);
    m_commonResolutions << QSize(1920, 1080);	// HD 1080, 1080i, 1080p
    m_commonResolutions << QSize(1920, 1200);
    m_commonResolutions << QSize(1920, 1280);
    m_commonResolutions << QSize(1920, 1400);
    m_commonResolutions << QSize(1920, 1440);
    m_commonResolutions << QSize(2048, 1152);
    m_commonResolutions << QSize(2048, 1280);
    m_commonResolutions << QSize(2048, 1536);
    m_commonResolutions << QSize(2048, 1556);
    m_commonResolutions << QSize(2160, 1440);
    m_commonResolutions << QSize(2304, 1440);
    m_commonResolutions << QSize(2304, 1728);
    m_commonResolutions << QSize(2538, 1080);
    m_commonResolutions << QSize(2560, 1080);
    m_commonResolutions << QSize(2560, 1440);
    m_commonResolutions << QSize(2560, 1600);
    m_commonResolutions << QSize(2560, 1700);
    m_commonResolutions << QSize(2560, 1800);
    m_commonResolutions << QSize(2560, 1920);
    m_commonResolutions << QSize(2560, 2048);
    m_commonResolutions << QSize(2732, 2048);
    m_commonResolutions << QSize(2736, 1824);
    m_commonResolutions << QSize(2800, 2100);
    m_commonResolutions << QSize(2880, 900);
    m_commonResolutions << QSize(2880, 1800);
    m_commonResolutions << QSize(3000, 2000);
    m_commonResolutions << QSize(3200, 1800);
    m_commonResolutions << QSize(3200, 2048);
    m_commonResolutions << QSize(3200, 2400);
    m_commonResolutions << QSize(3440, 1440);
    m_commonResolutions << QSize(3656, 2664);
    m_commonResolutions << QSize(3840, 2160);
    m_commonResolutions << QSize(3840, 2400);
    m_commonResolutions << QSize(4096, 2304);
    m_commonResolutions << QSize(4096, 3072);
    m_commonResolutions << QSize(4096, 3112);
    m_commonResolutions << QSize(5120, 2160);
    m_commonResolutions << QSize(5120, 2880);
    m_commonResolutions << QSize(5120, 3200);	// WHXGA
    m_commonResolutions << QSize(5120, 4096);
    m_commonResolutions << QSize(6400, 4096);
    m_commonResolutions << QSize(6400, 4800);
    m_commonResolutions << QSize(7680, 4320);	// 4320p
    m_commonResolutions << QSize(8192, 4608);
    m_commonResolutions << QSize(8192, 8192);
#else
    m_commonResolutions << QSize(320, 240);	// QVGA
    m_commonResolutions << QSize(352, 288);	// CIF
    m_commonResolutions << QSize(640, 480);	// VGA
    m_commonResolutions << QSize(704, 576);	// 4CIF
    m_commonResolutions << QSize(800, 600);	// SVGA
    m_commonResolutions << QSize(1024, 768);	// XGA
    m_commonResolutions << QSize(1280, 720);	// 720p
    m_commonResolutions << QSize(1408, 1152);	// 16CIF
    m_commonResolutions << QSize(1600, 900);	// 900p
    m_commonResolutions << QSize(1920, 1080);	// HD 1080, 1080i, 1080p
#endif

    m_initialized = false;
    updateAspectRatios();
}

bool CameraInfo::init(bool verboseResolutionQuery) {
    bool ok = false;
    m_webCamera = new cv::VideoCapture(m_cameraIndex + m_cameraBackend);
    if (m_webCamera && m_webCamera->open(m_cameraIndex)) {
        ok = queryResolutions(verboseResolutionQuery);
        if (!ok) {
            qDebug() << "Querying web camera resolutions failed";
        }
        m_webCamera->release();
    } else {
        qDebug() << "Problem creating and opening web camera" << m_cameraIndex;
    }
    updateAspectRatios();
    m_initialized = ok;
    return ok;
}

bool CameraInfo::isInitialized() {
    return m_initialized;
}

QList<QSize> CameraInfo::availableResolutions() {
    return m_availableResolutions;
}

QList<int> CameraInfo::knownAspectRatios() {
    return m_knownAspectRatios;
}

bool CameraInfo::queryResolutions(bool verbose) {
    int width = 0;
    int height = 0;
    QSize testRes;
    QSize smallestResolution;
    QSize largestResolution;
    int smallestIndex;
    int largestIndex;

    if (verbose) {
        qDebug() << "Querying web camera for supported resolutions";
    }

    // find smallest supported resolution
    testRes = m_commonResolutions.at(0);
    m_webCamera->set(CV_CAP_PROP_FRAME_WIDTH, testRes.width());
    m_webCamera->set(CV_CAP_PROP_FRAME_HEIGHT, testRes.height());
    width = m_webCamera->get(CV_CAP_PROP_FRAME_WIDTH);
    height = m_webCamera->get(CV_CAP_PROP_FRAME_HEIGHT);
    smallestResolution.setWidth(width);
    smallestResolution.setHeight(height);
    smallestIndex = m_commonResolutions.indexOf(smallestResolution);
    if (smallestIndex < 0) {
        /// @todo find out the nearest index at lower side if resolution not in list
        smallestIndex = 0;
    }
    emit queryProgressChanged(1);

    // find largest supported resolution
    // Actually this test can be done in query loop below: just wait until size is larger than the last found
    testRes = m_commonResolutions.at(m_commonResolutions.size() - 1);
    m_webCamera->set(CV_CAP_PROP_FRAME_WIDTH, testRes.width());
    m_webCamera->set(CV_CAP_PROP_FRAME_HEIGHT, testRes.height());
    width = m_webCamera->get(CV_CAP_PROP_FRAME_WIDTH);
    height = m_webCamera->get(CV_CAP_PROP_FRAME_HEIGHT);
    largestResolution.setWidth(width);
    largestResolution.setHeight(height);
    largestIndex = m_commonResolutions.indexOf(largestResolution);
    if (largestIndex < 0) {
        /// @todo find out the nearest index at higher side if resolution not in list
        largestIndex = m_commonResolutions.size() - 1;
    }
    emit queryProgressChanged(2);

    for (int i = smallestIndex; i < (largestIndex + 1); i++) {
        int queryPercent = ((double)(i - smallestIndex) / (double)(largestIndex - smallestIndex)) * (100 - 4) + 3;
        testRes = m_commonResolutions.at(i);
        m_webCamera->set(CV_CAP_PROP_FRAME_WIDTH, testRes.width());
        m_webCamera->set(CV_CAP_PROP_FRAME_HEIGHT, testRes.height());
        width = m_webCamera->get(CV_CAP_PROP_FRAME_WIDTH);
        height = m_webCamera->get(CV_CAP_PROP_FRAME_HEIGHT);
        if ((testRes.width() == width) && (testRes.height() == height)) {
            if (verbose) {
                qDebug() << "Web camera supports resolution" << width << "x" << height;
            }
            m_availableResolutions << testRes;
        } else {
            if (verbose) {
                qDebug() << "No support for resolution" << testRes.width() << "x" << testRes.height() <<"-- got " << width << "x" << height << "instead";
            }
            QSize newRes(width, height);
            if (!m_commonResolutions.contains(newRes) && !m_availableResolutions.contains(newRes)) {
                m_availableResolutions << newRes;
            }
        }
        emit queryProgressChanged(queryPercent);
    }

    // finally sort the available resolutions first by width, then by height
    std::sort(m_availableResolutions.begin(), m_availableResolutions.end(), CameraInfo::compareResolutionsWidthFirst);
    emit queryProgressChanged(100);

    return true;
}

void CameraInfo::updateAspectRatios() {
    int aspectRatio = 0;
    QSize res(0, 0);
    QListIterator<QSize> resIt(m_commonResolutions);
    QListIterator<QSize> availableResIt(m_availableResolutions);

    while (resIt.hasNext()) {
        res = resIt.next();
        aspectRatio = (double)res.width() / (double)res.height() * 10000;
        if (!m_knownAspectRatios.contains(aspectRatio)) {
            m_knownAspectRatios << aspectRatio;
        }
    }

    while (availableResIt.hasNext()) {
        res = availableResIt.next();
        aspectRatio = (double)res.width() / (double)res.height() * 10000;
        if (!m_knownAspectRatios.contains(aspectRatio)) {
            m_knownAspectRatios << aspectRatio;
        }
    }
    // sort aspect ratios in ascending order
    std::sort(m_knownAspectRatios.begin(), m_knownAspectRatios.end(), std::less<int>());
}

// static
bool CameraInfo::compareResolutionsWidthFirst(QSize res1, QSize res2) {
    if (res1.width() < res2.width()) {
        return true;
    }
    if (res1.width() == res2.width()) {
        if (res1.height() < res2.height()) {
            return true;
        }
    }
    return false;
}
