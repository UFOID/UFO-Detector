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

#include "camera.h"

Camera::Camera(int index, int width, int height)
{
    m_index = index;
    m_width = width;
    m_height = height;
    m_initialized = false;

    m_cameraInfo = new CameraInfo(m_index);
    connect(m_cameraInfo, SIGNAL(queryProgressChanged(int)), this, SIGNAL(queryProgressChanged(int)));

    std::cout << "Constructed camera with index " << m_index <<  std::endl;
}

bool Camera::init()
{
    m_webcam = new cv::VideoCapture(m_index);
    m_webcam->open(m_index);
    m_webcam->set(CV_CAP_PROP_FRAME_WIDTH, m_width);
    m_webcam->set(CV_CAP_PROP_FRAME_HEIGHT, m_height);
    int actualWidth = m_webcam->get(CV_CAP_PROP_FRAME_WIDTH);
    int actualHeight = m_webcam->get(CV_CAP_PROP_FRAME_HEIGHT);

    if ((actualWidth != m_width) || (actualHeight != m_height))
    {
        qDebug() << "Warning: requested web camera size" << m_width << "x" << m_height <<
                    "but got" << actualWidth << "x" << actualHeight;
    }

    if(m_webcam->isOpened())
    {
        m_webcam->read(frameToReturn);
        isReadingWebcam=true;
        threadReadFrame.reset(new std::thread(&Camera::readWebcamFrame, this));
    } else {
        return false;
    }
    m_initialized = true;
    return true;
}

bool Camera::isInitialized()
{
    return m_initialized;
}

void Camera::release()
{
    if (!m_webcam)
    {
        return;
    }
    stopReadingWebcam();
    m_webcam->release();
    m_initialized = false;
}

/*
 * Get current frame 
 */
cv::Mat Camera::getWebcamFrame()
{
    if (m_webcam && videoFrame.data && videoFrame.isContinuous())
	{
        mutex.lock();
        frameToReturn = videoFrame.clone();
        mutex.unlock();
        return frameToReturn;
    }
    else return frameToReturn;

}

/*
 * Stop the thread that continuously reads frame from webcam
 */
void Camera::stopReadingWebcam()
{
    if (threadReadFrame)
    {
        isReadingWebcam = false;
        mutex.lock();
        mutex.unlock();
        threadReadFrame->join();
        threadReadFrame.reset();
    }
}

/*
 * Continuously read frame in to videoFrame
 */
void Camera::readWebcamFrame()
{
    int yieldPauseUsec = 1000;    // e.g. at 25 FPS time between frames is 40,000 us
    while(m_webcam && isReadingWebcam && isWebcamOpen())
    {
        mutex.lock();
        m_webcam->read(videoFrame);
        mutex.unlock();
        // give other threads better possibility to run
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::microseconds(yieldPauseUsec));
    }
}

/*
 * Check if webcam is open from MainWindow
 */
bool Camera::isWebcamOpen()
{
    if (!m_webcam)
    {
        return false;
    }
    return m_webcam->isOpened();
}

int Camera::index()
{
    return m_index;
}

bool Camera::queryAvailableResolutions()
{
    if (!m_cameraInfo->isInitialized())
    {
        // first release current cv::VideoCapture & stop frame grab thread
        this->release();
        // CameraInfo::init() will reserve cv::VideoCapture and do the query
        m_cameraInfo->init();
        // now can continue using own cv::VideoCapture and restart frame grab thread
        this->init();
    }
    return true;
}

QList<QSize> Camera::availableResolutions()
{
    return m_cameraInfo->availableResolutions();
}

QList<int> Camera::knownAspectRatios()
{
    return m_cameraInfo->knownAspectRatios();
}
