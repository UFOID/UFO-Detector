#pragma once

#include "defines.h"

class TKalmanFilter {
public:
    cv::KalmanFilter *kalman;
    track_t deltatime;
    Point_t LastResult;

    TKalmanFilter(Point_t p, track_t dt = 0.2, track_t Accel_noise_mag = 0.5);

    ~TKalmanFilter();

    Point_t GetPrediction();

    Point_t Update(Point_t p, bool DataCorrect);
};
