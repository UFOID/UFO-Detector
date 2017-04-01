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
#include <condition_variable>
#include <QFile>

/*
 * == How to Use the Blocking Version of Camera::getWebcamFrame() ==
 *
 * It is possible to use the Camera mock object (not the real Camera object)
 * to get camera frames one by one for unit testing purposes. Here's how to
 * do it.
 *
 * 1. enable blocking by calling mockCamera_setFrameBlockingEnabled(true)
 * 2. start a frame consumer thread which will be calling Camera::getWebcamFrame()
 * 3. prepare a cv::Mat and put it into mockCameraNextFrame
 * 4. call mockCamera_releaseNextFrame() to get the next frame in the frame consumer thread
 * 5. the frame consumer thread now has the frame in mockCameraNextFrame
 * 6. to get the next frame to the consumer, go to step 3
 *
 * There's a usage example of this in ActualDetector unit test, more specifically
 * in TestActualDetector::mockCameraBlockNextFrame().
 */

cv::Mat mockCameraNextFrame;    ///< next frame to be given by Camera::getWebcamFrame()

/**
 * @brief Enable Camera::getWebcamFrame() blocking.
 */
std::atomic_bool mockCamera_blockFrameEnabled;

/**
 * @brief Mutex for blocking next call to Camera::getWebcamFrame().
 */
std::mutex mockCamera_blockerMutex;

/**
 * @brief Condition variable to do the actual waiting in Camera::getWebcamFrame().
 */
std::condition_variable_any mockCamera_blockerCond;

/**
 * @brief Enable blocking of Camera::getWebcamFrame() the next time it is called.
 * @note Be sure to not call Camera::getWebcamFrame() after this from main thread
 * because it will block. Call from a thread which is <i>intended/i> to block.
 */
void mockCamera_setFrameBlockingEnabled(bool enabled) {
    mockCamera_blockFrameEnabled = enabled;
    // wake up waiting threads
    if (!enabled) {
        mockCamera_blockerCond.notify_all();
    }
}

std::atomic<bool> isReadingVideo;

/**
 * @brief Release next camera frame from Camera::getWebcamFrame().
 */
void mockCamera_releaseNextFrame() {
    mockCamera_blockerCond.notify_all();
}

void startCameraFromVideo(QFile* videoFile){
    cv::VideoCapture webcam(videoFile->fileName().toStdString());
    if(!webcam.isOpened())
        qWarning() << "Cannot find test video file " + videoFile->fileName();


    isReadingVideo = true;
    int frames = webcam.get(cv::CAP_PROP_FRAME_COUNT);
    int fps = webcam.get(cv::CAP_PROP_FPS);
    for (int i=0; i<frames; i++)
    {
        webcam.read(mockCameraNextFrame);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/fps));

    }
    isReadingVideo = false;


}


Camera::Camera(int index, int width, int height) {
    Q_UNUSED(index);
    Q_UNUSED(width);
    Q_UNUSED(height);
    mockCamera_blockFrameEnabled = false;
}

bool Camera::init() {
    return true;
}

bool Camera::isInitialized() {
    return true;
}

void Camera::release() {
}

cv::Mat Camera::getWebcamFrame() {
    if (mockCamera_blockFrameEnabled) {
        //qDebug() << "lock mutex";
        mockCamera_blockerMutex.lock();
        mockCamera_blockerCond.wait(mockCamera_blockerMutex);
        //qDebug() << "unlock mutex";
        mockCamera_blockerMutex.unlock();
    }
    return mockCameraNextFrame;
}

void Camera::stopReadingWebcam() {

}

bool Camera::isWebcamOpen() {
    return true;
}

int Camera::index() {
    return 0;
}

bool Camera::queryAvailableResolutions() {
    return true;
}

QList<QSize> Camera::availableResolutions() {
    return QList<QSize>();
}

QList<int> Camera::knownAspectRatios() {
    return QList<int>();
}
