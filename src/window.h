//
// Created by dev on 9/7/23.
//

#ifndef DETECTOR_WINDOW_H
#define DETECTOR_WINDOW_H


#include <QObject>
#include <opencv2/core/mat.hpp>
#include "actualdetector.h"

using namespace cv;

class window : public QObject {
Q_OBJECT

public:
    window(ActualDetector *detector);

private slots:

    void update(Mat frame);

};


#endif //DETECTOR_WINDOW_H
