#include "Detector.h"

CDetector::CDetector(cv::Mat& gray)
{
    m_fg = gray.clone();
}

CDetector::~CDetector(void)
{
}


//----------------------------------------------------------------------
// Detector
//----------------------------------------------------------------------
void CDetector::DetectContour(cv::Mat& img, std::vector<cv::Rect>& Rects,std::vector<cv::Point2d>& centers, cv::Rect& croppedRect)
{
	m_rects.clear();
	m_centers.clear();
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
    cv::Mat edges=img.clone();
    //Canny(img, edges, 50, 190, 3);

    cv::findContours(edges,contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    if(contours.size()>0)
    {
        for(int i = 0; i < (int)contours.size(); i++)
        {
            cv::Rect r=cv::boundingRect(contours[i]);
            cv::Point tl = croppedRect.tl()+r.tl();
            cv::Point br = croppedRect.tl()+r.br();
            r=cv::Rect(tl,br);
            m_rects.push_back(r);
            m_centers.push_back((r.br()+r.tl())*0.5);

        }
    }
}

std::pair<std::vector<cv::Point2d>, std::vector<cv::Rect> > CDetector::Detect(cv::Mat& gray, cv::Rect &croppedRect)
{
    cv::dilate(gray, m_fg, getStructuringElement(cv::MORPH_RECT, cv::Size(15,15)));
    //imshow("Foreground",m_fg);
    DetectContour(m_fg,m_rects,m_centers,croppedRect);


    return make_pair(m_centers,m_rects);
}

const std::vector<cv::Rect>& CDetector::GetDetects() const
{
	return m_rects;
}
