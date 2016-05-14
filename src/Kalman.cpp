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

#pragma once
#include "Kalman.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TKalmanFilter::TKalmanFilter(Point2f pt,float dt,float Accel_noise_mag)
{
	//time increment (lower values makes target more "massive")
	deltatime = dt; //0.2

	// We don't know acceleration, so, assume it to process noise.
	// But we can guess, the range of acceleration values thich can be achieved by tracked object. 
	// Process noise. (standard deviation of acceleration: ì/ñ^2)
	// shows, woh much target can accelerate.
	//float Accel_noise_mag = 0.5; 

	//4 state variables, 2 measurements
	kalman = new KalmanFilter( 4, 2, 0 );  
	// Transition matrix
	kalman->transitionMatrix = (Mat_<float>(4, 4) << 1,0,deltatime,0,   0,1,0,deltatime,  0,0,1,0,  0,0,0,1);

	// init... 
	LastResult = pt;
	kalman->statePre.at<float>(0) = pt.x; // x
	kalman->statePre.at<float>(1) = pt.y; // y

	kalman->statePre.at<float>(2) = 0;
	kalman->statePre.at<float>(3) = 0;

	kalman->statePost.at<float>(0)=pt.x;
	kalman->statePost.at<float>(1)=pt.y;

	setIdentity(kalman->measurementMatrix);

	kalman->processNoiseCov=(Mat_<float>(4, 4) << 
		pow(deltatime,4.0)/4.0	,0						,pow(deltatime,3.0)/2.0		,0,
		0						,pow(deltatime,4.0)/4.0	,0							,pow(deltatime,3.0)/2.0,
		pow(deltatime,3.0)/2.0	,0						,pow(deltatime,2.0)			,0,
		0						,pow(deltatime,3.0)/2.0	,0							,pow(deltatime,2.0));


	kalman->processNoiseCov*=Accel_noise_mag;

	setIdentity(kalman->measurementNoiseCov, Scalar::all(0.1));

	setIdentity(kalman->errorCovPost, Scalar::all(.1));

}
//---------------------------------------------------------------------------
TKalmanFilter::~TKalmanFilter()
{
	delete kalman;
}

//---------------------------------------------------------------------------
Point2f TKalmanFilter::GetPrediction()
{
	Mat prediction = kalman->predict();
	LastResult=Point2f(prediction.at<float>(0),prediction.at<float>(1)); 
	return LastResult;
}
//---------------------------------------------------------------------------
Point2f TKalmanFilter::Update(Point2f p, bool DataCorrect)
{
	Mat measurement(2,1,CV_32FC1);
	if(!DataCorrect)
	{
		measurement.at<float>(0) = LastResult.x;  //update using prediction
		measurement.at<float>(1) = LastResult.y;
	}
	else
	{
		measurement.at<float>(0) = p.x;  //update using measurements
		measurement.at<float>(1) = p.y;
	}
	// Correction
	Mat estimated = kalman->correct(measurement);
	LastResult.x=estimated.at<float>(0);   //update using measurements
	LastResult.y=estimated.at<float>(1);
	return LastResult;
}
//---------------------------------------------------------------------------
