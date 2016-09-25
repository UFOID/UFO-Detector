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

#ifndef CTRACKER_H
#define CTRACKER_H

#include "Kalman.h"
#include "HungarianAlg.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;

/**
 * @brief Track of a moving object.
 */
class CTrack
{
public:
    vector<Point2d> trace;
    static size_t NextTrackID;
    size_t track_id;
    size_t skipped_frames;
    Point2d prediction;
    TKalmanFilter* KF;
    CTrack(Point2f p, float dt, float Accel_noise_mag);
    ~CTrack();
    int posCounter;
    int negCounter;
    int birdCounter;
    void getCount(int index);
};

/**
 * @brief Track object movement.
 */
class CTracker
{
public:

    float dt;

    float Accel_noise_mag;
    double dist_thres;
    int maximum_allowed_skipped_frames;
    int max_trace_length;

    vector<CTrack*> tracks;
    void Update(vector<Point2d>& detections);
    CTracker(float _dt, float _Accel_noise_mag, double _dist_thres, int _maximum_allowed_skipped_frames,int _max_trace_length);
    ~CTracker(void);
    bool removedTrackWithPositive;
    bool wasBird;
    void updateEmpty();
};

#endif // CTRACKER_H

