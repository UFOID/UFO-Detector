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

#include "BackgroundSubtract.h"

BackgroundSubtract::BackgroundSubtract()
{
	model = libvibeModelNew();
}

BackgroundSubtract::~BackgroundSubtract()
{
	libvibeModelFree(model);
}

void BackgroundSubtract::init(cv::Mat &image)
{
	int32_t width = image.size().width;
	int32_t height = image.size().height;
	int32_t stride = image.channels()*image.size().width;
	uint8_t *image_data = (uint8_t*)image.data;

	libvibeModelInit(model, image_data, width, height, stride);
}

void BackgroundSubtract::subtract(const cv::Mat &image, cv::Mat &foreground)
{
	uint8_t *image_data = (uint8_t*)image.data;
	uint8_t *segmentation_map = (uint8_t*)foreground.data;
	cv::Mat erodeElement = cv::getStructuringElement( 0, cv::Size( 5, 5 ), cv::Point( -1, -1 ) ); 
	cv::Mat dilateElement = cv::getStructuringElement( 0, cv::Size( 3, 3 ), cv::Point( -1, -1 ) ); 

	libvibeModelUpdate(model, image_data, segmentation_map);

	//cv::dilate(foreground, foreground, dilateElement, cv::Point(-1,-1), 1);
	cv::erode(foreground, foreground, erodeElement, cv::Point(-1, -1), 1);
	cv::dilate(foreground, foreground, dilateElement, cv::Point(-1,-1), 2);
}
