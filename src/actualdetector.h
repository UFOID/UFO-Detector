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

#ifndef ACTUALREC_H
#define ACTUALREC_H

#include <vector>
#include "recorder.h"
#include "config.h"
#include <time.h>
#include <QObject>
#include "opencv2/objdetect/objdetect.hpp"
#include <QImage>

class MainWindow;
class Recorder;
class Camera;

/**
 * @brief Main class to detect moving objects in video stream
 */
class ActualDetector : public QObject
{
 Q_OBJECT

public:
    ActualDetector(MainWindow *parent, Camera *cameraPtr = 0, Config *configPtr = 0);
    bool start();
    bool initialize();
    void stopThread();
    void setNoiseLevel(int level);
    void setThresholdLevel(int level);
    void setFilename(std::string msg);    
    void startRecording();
    Recorder* getRecorder();
    std::atomic<bool> willDisplayImage;

private:
    Recorder theRecorder;
    Camera* camPtr;
    Config* m_config;
    cv::Mat result, result_cropped;
    cv::Mat prev_frame, current_frame, next_frame;
    cv::Mat d1, d2, motion, treshImg;
    cv::Mat croppedImageGray;
    cv::Mat noiseLevel;
    cv::Rect rect;
    int numberOfChanges;
    int minAmountOfMotion, max_deviation;
    std::string pathname, pathnameThresh, fileDir;
    std::string ext;
    int imageCount;
    int thresholdLevel;
    int WIDTH;
    int HEIGHT;
    cv::Size m_cameraViewFrameSize;     ///< frame size of MainWindow's camera view
    int minPositiveRequired;
    bool willRecordWithRect;
    cv::CascadeClassifier birds_cascade;


    std::vector<cv::Point> region;
    std::string detectionAreaFile;

    std::atomic<bool> isMainThreadRunning;
    std::atomic<bool> willParseRectangle;
    bool isInNightMode;
    std::atomic<bool> startedRecording;
    bool willSaveImages;
    bool isCascadeFound;
    std::unique_ptr<std::thread> pThread;
    std::unique_ptr<std::thread> threadNightChecker;
    std::vector <cv::Rect> detectorRectVec;



    inline int detectMotion(const cv::Mat & motion, cv::Mat & result, cv::Mat & result_cropped,
                     std::vector<cv::Point> &region,
                     int max_deviation);
    bool parseDetectionAreaFile(std::string file_region, std::vector<cv::Point> &region);
    bool lightDetection(cv::Rect &rectangle, cv::Mat &croppedImage);
    void detectingThread();
    void detectingThreadHigh();
    void saveImg(std::string path, cv::Mat &image);
    std::pair<int, int> checkBrightness(int totalLight);
    void checkIfNight();
    void stopOnlyDetecting();

    /**
     * Check if single (!) object is a bird
     * @todo Improve to only look for single objects
     */
    bool checkIfBird();
    std::vector<cv::Rect> getConstantRecs(int totalLight);

    cv::Rect enlargeROI(cv::Mat &frm, cv::Rect &boundingBox, int padding);

signals:
    void positiveMessage();
    void negativeMessage();
    void errorReadingDetectionAreaFile();
	void broadcastOutputText(QString output_text);
	void progressValueChanged(int value);
    void updatePixmap(QImage img);

public slots:
    void onCameraViewFrameSizeChanged(QSize newSize);   ///< frame size of camera view has changed
};

#endif // ACTUALREC_H
