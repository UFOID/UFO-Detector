#pragma once

#include "Kalman.h"
#include "HungarianAlg.h"
#include "defines.h"
#include <iostream>
#include <vector>
#include <memory>
#include <array>

// --------------------------------------------------------------------------
class CTrack {
public:
    CTrack(const Point_t &p, const cv::Rect &rect, track_t dt, track_t Accel_noise_mag, size_t trackID)
            :
            track_id(trackID),
            skipped_frames(0),
            prediction(p),
            lastRect(rect),
            KF(p, dt, Accel_noise_mag) {
        negCounter = 0;
        posCounter = 0;
        birdCounter = 0;
    }

    track_t CalcDist(const Point_t &p) {
        Point_t diff = prediction - p;
        return sqrtf(diff.x * diff.x + diff.y * diff.y);
    }

    track_t CalcDist(const cv::Rect &r) {
        //cv::Rect rect = GetLastRect();

        std::array<track_t, 4> diff;
        diff[0] = prediction.x - lastRect.width / 2 - r.x;
        diff[1] = prediction.y - lastRect.height / 2 - r.y;
        diff[2] = static_cast<track_t>(lastRect.width - r.width);
        diff[3] = static_cast<track_t>(lastRect.height - r.height);

        track_t dist = 0;
        for (size_t i = 0; i < diff.size(); ++i) {
            dist += diff[i] * diff[i];
        }
        return sqrtf(dist);
    }

    void Update(const Point_t &p, const cv::Rect &rect, bool dataCorrect, size_t max_trace_length) {
        KF.GetPrediction();
        prediction = KF.Update(p, dataCorrect);

        if (dataCorrect) {
            lastRect = rect;
        }

        if (trace.size() > max_trace_length) {
            trace.erase(trace.begin(), trace.end() - max_trace_length);
        }

        trace.push_back(prediction);
    }

    std::vector<Point_t> trace;
    size_t track_id;
    size_t skipped_frames;

    int posCounter;
    int negCounter;
    int birdCounter;

    cv::Rect GetLastRect() {
        return cv::Rect(
                static_cast<int>(prediction.x - lastRect.width / 2),
                static_cast<int>(prediction.y - lastRect.height / 2),
                lastRect.width,
                lastRect.height);
    }

private:
    Point_t prediction;
    cv::Rect lastRect;
    TKalmanFilter KF;
};

// --------------------------------------------------------------------------
class CTracker {
public:
    CTracker(track_t dt_, track_t Accel_noise_mag_, track_t dist_thres_ = 60,
             size_t maximum_allowed_skipped_frames_ = 10, size_t max_trace_length_ = 10);

    ~CTracker(void);

    enum DistType {
        CentersDist = 0,
        RectsDist = 1
    };

    std::vector<std::unique_ptr<CTrack>> tracks;

    void Update(const std::vector<cv::Point2d> &detections, const std::vector<cv::Rect> &rects, DistType distType);

    bool removedTrackWithPositive;
    bool wasBird;

    void updateEmpty();

private:
    track_t dt;

    track_t Accel_noise_mag;


    track_t dist_thres;
    size_t maximum_allowed_skipped_frames;
    size_t max_trace_length;

    size_t NextTrackID;
};
