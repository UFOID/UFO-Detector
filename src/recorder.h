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

#ifndef RECORDER_H
#define RECORDER_H

#include "config.h"
#include "camera.h"
#include "actualdetector.h"
#include <QDomDocument>
#include <QFile>
#include <QObject>
#include <QProcess>
#include <QTime>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <atomic>
#include <thread>
#include <memory>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define OUTPUT_FPS 25

using namespace cv;
using namespace std;
using frame_period = std::chrono::duration<long long, std::ratio<1, OUTPUT_FPS>>;
using hr_duration = std::chrono::high_resolution_clock::duration;

class ActualDetector;

/**
 * @brief The class for recording videos from web camera
 */
class Recorder : public QObject {
Q_OBJECT

public:
    explicit Recorder(ActualDetector* parent=0, Camera* cameraPtr = 0, Config* configPtr = 0);
    void startRecording(cv::Mat &f);
    void stopRecording(bool willSaveVideo);
    void setRectangle(cv::Rect &r, bool isRed);

#ifndef _UNIT_TEST_
private:
#endif
    const int DEFAULT_CODEC = 0;

    Camera* camPtr;
    Config* m_config;
    cv::VideoWriter theVideoWriter;
    cv::Mat frameToRecord;
    cv::Mat firstFrame;
    cv::Mat secondFrame;
    cv::Rect motionRectangle;
    cv::Scalar color;
    cv::Size resolutionRecording;
    cv::Size resolutionThumbnail;
    int m_defaultThumbnailSideLength;   ///< bounding square side length for thumbnail image

    std::string pathDirectory;
    std::string ext;

    int delayFound = 0;
    int totalDelay= 0;

    std::unique_ptr<std::thread> recThread;
    std::unique_ptr<std::thread> frameUpdateThread;
    std::atomic<bool>  recording;
    bool willBeSaved;
    bool withRectangle;

    std::vector<QProcess*> vecProcess;
    std::vector<QString> vecTempVideoFile;

    QDomDocument documentXML;
    QFile resultDataFile;
    QDomElement rootXML;

    std::chrono::high_resolution_clock::time_point prev, current;

    int codec;
    int codecSetting;

    void recordThread();
    void readFrameThread();
    void readFrameThreadWithoutRect();
    void saveLog(std::string dateTime, QString videoLength);

public slots:
    void reloadResultDataFile();

#ifndef _UNIT_TEST_
private slots:
#endif
    void finishedRecording(QString picDir, QString filename);
    void finishedProcess();

signals:
    void updateListWidget(QString file, QString date, QString length);
    void recordingStarted();
    void recordingStopped();
    void finishedRec(QString picDir, QString filename);



};

#endif // RECORDER_H
