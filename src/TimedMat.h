//
// Created by UFO ID on 01/01/2024.
//

#ifndef DETECTOR_TIMEDMAT_H
#define DETECTOR_TIMEDMAT_H


#include <opencv2/opencv.hpp>

struct TimedMat {
    cv::Mat frame;
    long timestamp;

    TimedMat(const cv::Mat &mat, long ts)
            : frame(mat), timestamp(ts) {}
};

#endif //DETECTOR_TIMEDMAT_H
