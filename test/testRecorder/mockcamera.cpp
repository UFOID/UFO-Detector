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
