//
// Created by dev on 9/7/23.
//

#include <opencv2/highgui.hpp>
#include "window.h"

window::window(ActualDetector *detector) : QObject(detector) {

    QObject::connect(detector, SIGNAL(updateFrame(Mat)), this, SLOT(update(Mat)));
    namedWindow("window", WINDOW_AUTOSIZE);
}

void window::update(Mat image) {
    imshow("window", image);
    waitKey(10);
}