/**
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

#include "Detector.h"
using namespace cv;
using namespace std;

CDetector::CDetector(Mat& gray)
{
	fg=gray.clone();
	bs=new BackgroundSubtract;
	bs->init(gray);
	vector<Rect> rects;
	vector<Point2d> centers;
	
}

//----------------------------------------------------------------------
// Detector
//----------------------------------------------------------------------
void CDetector::DetectContour(Mat& img, vector<Rect>& Rects,vector<Point2d>& centers, Rect& croppedRect)
{
	double area=0;
	Rects.clear();
	centers.clear();
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat edges=img.clone();
    //Canny(img, edges, 50, 190, 3);

    findContours(edges,contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point());
    //findContours( edges, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
    if(contours.size()>0)
	{
		for( int i = 0; i < contours.size(); i++ )
		{
            Rect r=cv::boundingRect(contours[i]);
            Point tl = croppedRect.tl()+r.tl();
            Point br = croppedRect.tl()+r.br();
            r=Rect(tl,br);
            Rects.push_back(r);
            centers.push_back((r.br()+r.tl())*0.5);
		}
	}
}

/*
 * Find all object in binary image
 * Return pair of vectors that contains the center point of each object and the rectangle around each object 
 * Problem: Sometimes one object is detected as two because the binary image does not show one object as a continuous area 
 * TODO: Improve the detection of objects
 */
pair< vector<Point2d>,vector<Rect> > CDetector::Detect(Mat& gray, Rect& croppedRect){

        //bs->subtract(gray,fg);
		// rects - bounding rectangles
		// centers - centers of bounding rectangles
        /*
		Mat fg2;
		fg.convertTo(fg2,CV_32FC1);
		cv::GaussianBlur(fg2,fg2,Size(5,5),1.0);
		cv::Laplacian(fg2,fg2,CV_32FC1);

		normalize(fg2,fg2,0,255,cv::NORM_MINMAX);
		fg2.convertTo(fg2,CV_8UC1);
		cv::applyColorMap(fg2,fg2,COLORMAP_JET);
		imshow("Foreground",fg2);
        */
        //cv::Mat dilateElement = cv::getStructuringElement( 0, cv::Size( 3, 3 ), cv::Point( -1, -1 ) );
        //cv::dilate(gray, fg, dilateElement, cv::Point(-1,-1), 2);
        dilate(gray, fg, getStructuringElement(MORPH_RECT, Size(15,15)));
        //imshow("Foreground",fg);
        DetectContour(fg,rects,centers,croppedRect);
        return make_pair(centers,rects);
}

vector<Rect> CDetector::getRecs(){
    return rects;
}

CDetector::~CDetector(void)
{
	delete bs;
}
