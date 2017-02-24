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
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <chrono>
#include <stdio.h>
#include "camera.h"
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QtXml>
#include "Ctracker.h"
#include "Detector.h"
#include "planechecker.h"

using namespace cv;

class Recorder;

/**
 * @brief Main class to detect moving objects in video stream.
 */
class ActualDetector : public QObject
{
 Q_OBJECT

public:
    ActualDetector(Camera *cameraPtr = 0, Config *configPtr = 0, QObject *parent = 0);
    ~ActualDetector();

    /**
     * @brief Initialize ActualDetector. Need to be called before calling start().
     * @return
     */
    bool initialize();

    /**
     * @brief Start the detection process.
     */
    bool start();

    /**
     * @brief Stop the detection process.
     */
    void stopThread();

    void setNoiseLevel(int level);
    void setThresholdLevel(int level);
    void setFilename(std::string msg);
    void startRecording();
    void setPlaneChecker(PlaneChecker* planeChecker);
    Recorder* getRecorder();

    /**
     * @brief Set whether to show camera video during detection.
     *
     * Video showing is done by emitting updatePixmap() signal for each video frame.
     * Actual showing must be done by the camera view in the UI side.
     * When camera video is not shown, no updatePixmap() signals are emitted.
     *
     * @param show true = show video, false = don't show video
     */
    void setShowCameraVideo(bool show);

#ifndef _UNIT_TEST_
private:
#endif
    Recorder* m_recorder;
    Camera* m_camPtr;
    Config* m_config;
    cv::Mat m_resultFrame;
    cv::Mat m_resultFrameCropped;
    cv::Mat m_prevFrame;
    cv::Mat m_currentFrame;
    cv::Mat m_nextFrame;
    std::atomic<bool> m_showCameraVideo; ///< whether the camera video is shown (updatePixmap signal emitted)
    QImage m_cameraViewImage;   ///< image to be given out with signal updatePixmap()
    cv::Mat m_d1;
    cv::Mat m_d2;
    cv::Mat m_motion;
    cv::Mat m_treshImg;
    cv::Mat m_croppedImageGray;
    cv::Mat m_noiseLevel;
    cv::Rect m_rect;
    int m_minAmountOfMotion;
    int m_maxDeviation;
    std::string m_resultImageDirNameBase; ///< base for result image folder name
    std::string m_pathNameThresh;
    std::string m_resultImageDirName;   ///< complete result image folder name
    std::string m_savedImageExtension;
    int m_imageCount;
    int m_thresholdLevel;
    int m_cameraWidth;
    int m_cameraHeight;
    int m_minPositiveRequired;
    bool m_wasPlane;
    const unsigned int MAX_OBJECTS_IN_FRAME = 10;
    const int CLASSIFIER_DIMENSION_SIZE = 30;
    bool m_willRecordWithRect;
    cv::CascadeClassifier m_birdsCascade;
    PlaneChecker* m_planeChecker;


    std::vector<cv::Point> m_region;
    std::string m_detectionAreaFile;

    std::atomic<int> m_numberOfPlanes;
    std::atomic<bool> m_isMainThreadRunning;
    std::atomic<bool> m_willParseRectangle;
    bool m_isInNightMode;
    std::atomic<bool> m_startedRecording;
    bool m_willSaveImages;
    bool m_isCascadeFound;
    std::unique_ptr<std::thread> m_mainThread;
    std::unique_ptr<std::thread> m_nightCheckerThread;
    std::vector <cv::Rect> m_detectorRectVec;


    inline int detectMotion(const cv::Mat & m_motion, cv::Mat & m_resultFrame, cv::Mat & m_resultFrameCropped,
                     std::vector<cv::Point> &m_region,
                     int m_maxDeviation);
    bool parseDetectionAreaFile(std::string file_region, std::vector<cv::Point> &m_region);
    bool lightDetection(cv::Rect &rectangle, cv::Mat &croppedImage);
    void detectingThread();
    void detectingThreadHigh();
    void saveImg(std::string path, cv::Mat &image);
    std::pair<int, int> checkBrightness(int totalLight);
    void checkIfNight();
    void stopOnlyDetecting();

    /**
     * @brief Check if single (!) object is a bird.
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
    void checkPlane();

private slots:
    void setAmountOfPlanes(int amount);
};

#endif // ACTUALREC_H
