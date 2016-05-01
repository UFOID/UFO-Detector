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

#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/highgui/highgui.hpp>
#include <mutex>
#include <thread>
#include <atomic>
#include <memory>


class Camera
{
public:
    Camera(int index, int width, int height);
    cv::Mat getWebcamFrame();
    void stopReadingWebcam();
    bool isWebcamOpen();

private:
    std::atomic<bool> isReadingWebcam;
    cv::VideoCapture webcam;
    cv::Mat videoFrame;
    cv::Mat frameToReturn;
    std::mutex mutex;
    void readWebcamFrame();
    std::unique_ptr<std::thread> threadReadFrame;
};

#endif // CAMERA_H
