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
#include <ctime>
#include <QObject>
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <chrono>
#include <cstdio>
#include "Camera.h"
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include "Ctracker.h"
#include "detectorstate.h"
#include "datamanager.h"

using namespace cv;

class Recorder;

/**
 * @brief Main class to detect moving objects in video stream.
 *
 * @todo use constant frame rate for detection (though exposure time changes)
 */
class ActualDetector : public QObject {
Q_OBJECT

public:
    ActualDetector(Camera *camera, Config *config, DataManager *dataManager, QObject *parent = nullptr);

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

    Recorder *getRecorder();

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
    std::unique_ptr<Recorder> m_recorder;
    Camera *m_camPtr;
    Config *m_config;
    cv::Mat m_resultFrame;
    cv::Mat m_prevFrame;
    cv::Mat m_currentFrame;
    cv::Mat m_nextFrame;
    std::atomic<bool> m_showCameraVideo; ///< whether the camera video is shown (updatePixmap signal emitted)
    cv::Mat m_d1;
    cv::Mat m_d2;
    cv::Mat m_motion;
    cv::Mat m_treshImg;
    cv::Mat m_croppedImageGray;
    cv::Mat m_noiseLevel;
    cv::Rect m_rect;
    DataManager *m_dataManager;
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
    std::unique_ptr<DetectorState> state;
    const unsigned int MAX_OBJECTS_IN_FRAME = 10;
    const int CLASSIFIER_DIMENSION_SIZE = 30;
    bool m_willRecordWithRect;
    cv::CascadeClassifier m_birdsCascade;


    std::vector<cv::Point> m_region;
    std::string m_detectionAreaFile;

    std::atomic<bool> m_isMainThreadRunning;
    std::atomic<bool> m_willParseRectangle;
    bool m_isInNightMode;
    std::atomic<bool> m_startedRecording;
    bool m_willSaveImages;
    bool m_isCascadeFound;
    std::unique_ptr<std::thread> m_mainThread;
    std::unique_ptr<std::thread> m_nightCheckerThread;
    std::vector<cv::Rect> m_detectorRectVec;


    inline int detectMotion(const Mat &m_motion, Mat &m_resultFrame, vector<Point> &m_region, int m_maxDeviation);

    /**
     * @brief Initialize detection area.
     * Currently combining all defined detection areas into a single one.
     * @return true on success, false on failure
     */
    bool initDetectionArea();

    bool lightDetection(cv::Rect &rectangle, cv::Mat &croppedImage);

    void detectingThread();

    void saveImg(std::string path, cv::Mat &image);

    static std::pair<int, int> checkBrightness(int totalLight);

    /**
     * @brief Check if single (!) object is a bird.
     * @todo Improve to only look for single objects
     */
    bool checkIfBird();

    cv::Rect enlargeROI(cv::Mat &frm, cv::Rect &boundingBox, int padding);

signals:

    void positiveMessage();

    void negativeMessage();

    void broadcastOutputText(QString output_text);

    void progressValueChanged(int value);

    void checkPlane();

    void updateFrame(Mat frame);

private slots:

    void recordingFinished();
};

#endif // ACTUALREC_H
