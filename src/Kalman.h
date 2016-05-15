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

#ifndef KALMAN_H
#define KALMAN_H

#include "opencv2/opencv.hpp"
#include <opencv/cv.h>
using namespace cv;
using namespace std;

/**
 * @brief Implementation of Kalman filter
 *
 * @see http://www.morethantechnical.com/2011/06/17/simple-kalman-filter-for-tracking-using-opencv-2-2-w-code/
 */
class TKalmanFilter
{
public:
	KalmanFilter* kalman;
	double deltatime;	// time increment
	Point2f LastResult;
	TKalmanFilter(Point2f p,float dt=0.2,float Accel_noise_mag=0.5);
	~TKalmanFilter();
	Point2f GetPrediction();
	Point2f Update(Point2f p, bool DataCorrect);
};

#endif // KALMAN_H
