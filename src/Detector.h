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
#include "opencv2/opencv.hpp"
#include "BackgroundSubtract.h"
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;
class CDetector
{
private:
        void DetectContour(Mat& img, vector<Rect>& Rects, vector<Point2d>& centers, Rect &croppedRect);
	BackgroundSubtract* bs;
	vector<Rect> rects;
	vector<Point2d> centers;
	Mat fg;
public:
	CDetector(Mat& gray);
        pair< vector<Point2d>,vector<Rect> >  Detect(Mat& gray, Rect &croppedRect);
        vector<Rect> getRecs();
	~CDetector(void);
};

