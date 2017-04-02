#pragma once
#include <iostream>
#include <vector>
#include "defines.h"
#include "opencv2/opencv.hpp"

class CDetector
{
private:
    void DetectContour(cv::Mat& img, std::vector<cv::Rect>& Rects,std::vector<cv::Point2d>& centers, cv::Rect& croppedRect);

	std::vector<cv::Rect> m_rects;
    std::vector<cv::Point2d> m_centers;
	cv::Mat m_fg;

public:
	CDetector(cv::Mat& gray);
    std::pair<std::vector<cv::Point2d>,std::vector<cv::Rect> > Detect(cv::Mat& gray, cv::Rect &croppedRect);
	~CDetector(void);

	const std::vector<cv::Rect>& GetDetects() const;
};
