//
// Created by UFO ID on 03/12/2023.
//

#include "Camera.h"

bool Camera::startCamera() {

    camera.open(
            "v4l2src io-mode=dmabuf device=/dev/video0 ! video/x-raw, format=(string)UYVY, width=(int)1920, height=(int)1080 ! appsink",
            cv::CAP_GSTREAMER);


    if (!camera.isOpened()) {
        std::cerr << "Error opening video capture." << std::endl;
        return false;
    }

    cv::Mat frame;
    camera >> frame;
    if (frame.empty()) {
        std::cerr << "Reading Frame is empty" << std::endl;
        return false;
    }

    isRunning = true;
    m_frameUpdateThread = std::make_unique<std::thread>(&Camera::captureThread, this);
    return true;
}

void Camera::captureThread() {
    using namespace std::chrono;
    auto lastTime = steady_clock::now();
    int frameCount = 0;

    while (isRunning) {
        cv::Mat frame;
        camera >> frame;
        if (frame.empty()) {
            std::cout << "Reading Frame is empty" << std::endl;
            continue;
        }
        cv::cvtColor(frame, frame, cv::COLOR_YUV2BGR_UYVY);

        auto now = std::chrono::system_clock::now();
        long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        bufferDetector.push(TimedMat(frame, timestamp));

        if (!stoppedRecorderBuffer) {
            bufferRecorder.push(TimedMat(frame.clone(), timestamp));
        }


//        frameCount++;
//        auto currentTime = steady_clock::now();
//        duration<double> elapsed = currentTime - lastTime;
//
//        if (elapsed.count() >= 1.0) {  // Every second
//            double fps = frameCount / elapsed.count();
//            std::cout << "Thread  FPS: " << fps << std::endl;
//            frameCount = 0;
//            lastTime = currentTime;
//        }
    }
}

TimedMat Camera::getFrameRecorder() {

    return bufferRecorder.frontAndPop();
}

TimedMat Camera::getFrameDetector() {

    return bufferDetector.frontAndPop();
}

void Camera::startRecording() {
    // todo depending on available memory, set max size. For Raspberry Pi 4, 600 is the max. For Raspberry Pi 5, 800 is ok
    bufferRecorder.setMaxSize(800);
}

void Camera::stopRecording() {
    for (int i = 0; i < bufferRecorder.size(); i++) {
        TimedMat timedMat = bufferRecorder.frontAndPop();
        timedMat.frame.release();
    }

    bufferRecorder.clear();
    bufferRecorder.setMaxSize(200);
    stoppedRecorderBuffer = false;
}

/**
 * Stops the recorder buffer from filling up
 * This is called after all the frames needed for the video have been added to the buffer
 */
void Camera::stopRecorderBuffer() {
    // todo devices with less memory will need to stop the buffer from filling up
    // however, on faster devices (e.g. Raspberry Pi 5), stopping the buffer can end up in the buffer being empty and the video recording never finishing
//    stoppedRecorderBuffer = true;
}