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

#include "camera.h"

cv::Mat mockCameraNextFrame;    ///< next frame which Camera::getWebcamFrame() will give

Camera::Camera(int index, int width, int height) {
    Q_UNUSED(index);
    Q_UNUSED(width);
    Q_UNUSED(height);
}

bool Camera::init() {
    return true;
}

bool Camera::isInitialized() {
    return true;
}

void Camera::release() {
}

cv::Mat Camera::getWebcamFrame() {
    return mockCameraNextFrame;
}

void Camera::stopReadingWebcam() {

}

bool Camera::isWebcamOpen() {
    return true;
}

int Camera::index() {
    return 0;
}

bool Camera::queryAvailableResolutions() {
    return true;
}

QList<QSize> Camera::availableResolutions() {
    return QList<QSize>();
}

QList<int> Camera::knownAspectRatios() {
    return QList<int>();
}
