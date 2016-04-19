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

#ifndef RECORDER_H
#define RECORDER_H

#include <opencv2/highgui/highgui.hpp>
#include <atomic>
#include <QDomDocument>
#include <QFile>
#include <thread>
#include <QObject>
#include <QProcess>
#include <memory>

class ActualDetector;
class Camera;

class Recorder : public QObject {
Q_OBJECT

public:
    explicit Recorder(ActualDetector* parent=0, Camera* cameraPtr = 0);
    void setup(cv::Mat &f);
    void stopRecording(bool b);
    void setRectangle(cv::Rect &r, bool isRed);

private:
	const int DEFAULT_CODEC = 0;

    Camera* camPtr;
    cv::VideoWriter theVideoWriter;
    cv::Mat frameToRecord;
    cv::Mat firstFrame;
    cv::Mat secondFrame;
    cv::Rect motionRectangle;
    cv::Scalar color;
    cv::Size resolutionRecording;
    cv::Size resolutionThumbnail;

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
    std::vector<QString> vecString;

    QDomDocument documentXML;
    QFile fileXML;
    QDomElement rootXML;

    std::chrono::high_resolution_clock::time_point prev, current;

    int codec;
    int codecSetting;

    void recordThread();
    void readFrameThread();
    void readFrameThreadWithoutRect();
    void saveLog(std::string dateTime, QString videoLength);

public slots:
    void reloadXML();

private slots:
    void finishedRecording(QString picDir, QString filename);
    void finishedProcess();

signals:
    void updateListWidget(QString file, QString date, QString length);
    void recordingStarted();
    void recordingStopped();
    void finishedRec(QString picDir, QString filename);



};

#endif // RECORDER_H
