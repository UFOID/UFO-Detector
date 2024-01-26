//
// Created by UFO ID on 03/12/2023.
//

#ifndef DETECTOR_NEW_CAMERA_H
#define DETECTOR_NEW_CAMERA_H

#include <opencv2/highgui/highgui.hpp>
#include <queue>
#include <thread>
#include <iostream>
#include <atomic>
#include <opencv2/imgproc.hpp>
#include "ThreadSafeQueue.cpp"
#include "TimedMat.h"


class Camera {

public:

    bool startCamera();

    void stopCamera();

    TimedMat getFrameDetector();

    TimedMat getFrameRecorder();

    void startRecording();

    void stopRecording();

    void stopRecorderBuffer();

private:

    cv::VideoCapture camera;
    ThreadSafeQueue<TimedMat> bufferDetector = ThreadSafeQueue<TimedMat>(60);
    ThreadSafeQueue<TimedMat> bufferRecorder = ThreadSafeQueue<TimedMat>(200);
    std::atomic<bool> isRunning;

    std::unique_ptr<std::thread> m_frameUpdateThread;


    void captureThread();


    std::atomic<bool> stoppedRecorderBuffer;
};


#endif //DETECTOR_NEW_CAMERA_H
