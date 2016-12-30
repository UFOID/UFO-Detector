#pragma once
#include "BackgroundSubtract.h"
#include <iostream>
#include <vector>
#include "defines.h"
#include "opencv2/opencv.hpp"

class CDetector
{
private:
    void DetectContour(cv::Mat& img, std::vector<cv::Rect>& Rects,std::vector<cv::Point2d>& centers, cv::Rect& croppedRect);

	std::unique_ptr<BackgroundSubtract> m_bs;
	std::vector<cv::Rect> m_rects;
    std::vector<cv::Point2d> m_centers;
	cv::Mat m_fg;

	cv::Size m_minObjectSize;

public:
	CDetector(cv::Mat& gray);
    std::pair<std::vector<cv::Point2d>,std::vector<cv::Rect> > Detect(cv::Mat& gray, cv::Rect &croppedRect);
	~CDetector(void);

	void SetMinObjectSize(cv::Size minObjectSize);

	const std::vector<cv::Rect>& GetDetects() const;
};
