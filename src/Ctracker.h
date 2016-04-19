#pragma once
#include "Kalman.h"
#include "HungarianAlg.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;

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

