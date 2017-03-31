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
#include "videobuffer.h"
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
#include <QMutex>
#include <atomic>
#include <thread>
#include <memory>
#include <iostream>
#include <chrono>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define OUTPUT_FPS 25
#define VIDEO_BUFFER_CAPACITY (OUTPUT_FPS * 5)

using namespace cv;
using namespace std;
using frame_period = std::chrono::duration<long long, std::ratio<1, OUTPUT_FPS>>;
using hr_duration = std::chrono::high_resolution_clock::duration;

class ActualDetector;

/**
 * @brief The class for recording videos from web camera.
 */
class Recorder : public QObject {
Q_OBJECT

public:
    explicit Recorder(Camera* cameraPtr = 0, Config* configPtr = 0);
    void startRecording(cv::Mat &firstFrame);
    void stopRecording(bool willSaveVideo);
    void setRectangle(cv::Rect &r, bool isRed);

#ifndef _UNIT_TEST_
private:
#endif
    const int DEFAULT_CODEC = 0;

    Camera* m_camera;
    Config* m_config;
    cv::VideoWriter m_videoWriter;
    cv::Mat m_firstFrame;
    VideoBuffer* m_videoBuffer;
    cv::Rect m_motionRectangle;
    cv::Scalar m_objectRectangleColor;  ///< color of object rectangle, changes each time
    cv::Scalar m_objectPositiveColor;   ///< color used to draw rectangle around a positive detection object
    cv::Scalar m_objectNegativeColor;   ///< color used to draw rectangle around a negative detection object
    cv::Size m_videoResolution;     ///< resolution of video to be saved
    cv::Size m_thumbnailResolution; ///< resolution of video thumbnail image
    int m_defaultThumbnailSideLength;   ///< bounding square side length for thumbnail image
    QString m_resultVideoDirName;     ///< result data directory name
    QString m_thumbnailDirName;      ///< name of thumbnail folder (without slashes)
    QString m_videoFileExtension;
    QString m_thumbnailExtension;

    std::unique_ptr<std::thread> m_recorderThread;
    std::unique_ptr<std::thread> m_frameUpdateThread;
    std::atomic<bool> m_recording;
    bool m_willSaveVideo;       ///< whether to save video or reject it
    bool m_drawRectangles;      ///< whether or not to draw rectangles around detected objects

    std::vector<QProcess*> m_encoderProcesses;
    std::vector<QString> m_tempVideoFiles;

    QDomDocument m_resultDataDomDocument;
    QFile m_resultDataFile;
    QDomElement m_resultDataRootElement;

    void recordThread();

    /**
     * @brief camera frame reader thread
     *
     * @todo use a single source of frames
     */
    void readFrameThread();

    /**
     * @brief Save information about the video to result data file.
     * Emits resultDataSaved() signal.
     * @param dateTime format: yyyy-MM-dd--hh-mm-ss
     * @param videoLength format: mm:ss
     */
    void saveResultData(QString dateTime, QString videoLength);

public slots:
    void reloadResultDataFile();

#ifndef _UNIT_TEST_
private slots:
#endif
    /**
     * @brief Creates a new QProcess that encodes the temporary raw video with ffmpeg/avconv.
     *
     * @note this method NEEDS to be called via signal-slot system, I guess in order
     * to run it in the main thread. If this is called from detecting thread via direct
     * method call then can't get signals from the encoder process.
     */
    void startEncodingVideo(QString tempVideoFileName, QString targetVideoFileName);

    void onVideoEncodingFinished();

signals:
    void resultDataSaved(QString file, QString date, QString length);
    void recordingStarted();
    void recordingFinished();
    void videoEncodingRequested(QString tempVideoName, QString targetVideoName);
};

#endif // RECORDER_H
