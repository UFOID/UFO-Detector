/**
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
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>

/*
 * Main Camera class to handle reading of frames from multiple threads
 */
Camera::Camera(int index, int width, int height)
{
    webcam.open(index);
    webcam.set(CV_CAP_PROP_FRAME_WIDTH, width);
    webcam.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    int actualWidth = webcam.get(CV_CAP_PROP_FRAME_WIDTH);
    int actualHeight = webcam.get(CV_CAP_PROP_FRAME_HEIGHT);

    if ((actualWidth != width) || (actualHeight != height))
    {
        qDebug() << "Warning: requested web camera size" << width << "x" << height <<
                    "but got" << actualWidth << "x" << actualHeight;
    }

    if(webcam.isOpened())
	{
        webcam.read(frameToReturn);
        isReadingWebcam=true;
        threadReadFrame.reset(new std::thread(&Camera::readWebcamFrame, this));
    }

    std::cout << "Constructed camera with index " << index <<  std::endl;
}

/*
 * Get current frame 
 */
cv::Mat Camera::getWebcamFrame()
{

    if (videoFrame.data && videoFrame.isContinuous())
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
    while(isReadingWebcam)
	{
        mutex.lock();
        webcam.read(videoFrame);
        mutex.unlock();
    }
}

/*
 * Check if webcam is open from MainWindow
 */
bool Camera::isWebcamOpen()
{
    return webcam.isOpened();
}

