#include "camera.h"

Camera::Camera(int index, int width, int height) {

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
    return cv::Mat();
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
