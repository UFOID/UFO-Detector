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

#include "recorder.h"

#include <QElapsedTimer>
#include <QVector>
#include <memory>

Recorder::Recorder(Camera *cameraPtr, Config *configPtr) :
        m_camera(cameraPtr), m_config(configPtr), m_firstFrame(TimedMat(Mat(), 0)) {
    qDebug() << "Creating recorder";
    const int width = m_config->cameraWidth();
    const int height = m_config->cameraHeight();
    m_drawRectangles = m_config->resultVideoWithObjectRectangles();
    m_resultVideoDirName = m_config->resultVideoDir();
    m_thumbnailDirName = "thumbnails";
    m_thumbnailExtension = ".jpg";

    QDir dir(QString(m_resultVideoDirName + "/" + m_thumbnailDirName));
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    m_videoFileExtension = ".mp4";
    m_objectPositiveColor = Scalar(255, 0, 0);
    m_objectNegativeColor = Scalar(0, 0, 255);
    m_objectRectangleColor = m_objectPositiveColor;
    m_videoResolution = Size(width, height);

    double aspectRatio = (double) width / (double) height;
    m_defaultThumbnailSideLength = 160;
    int thumbnailWidth = m_defaultThumbnailSideLength;
    int thumbnailHeight = qBound(m_defaultThumbnailSideLength / 2,
                                 (int) (thumbnailWidth / aspectRatio), m_defaultThumbnailSideLength);
    m_thumbnailResolution = Size(thumbnailWidth, thumbnailHeight);

    m_recording = false;
    m_stopRecording = false;
    QObject::connect(this, SIGNAL(videoRecordingTimeout()), this, SLOT(stopRecordingAndSaveVideo()));
    QObject::connect(this, SIGNAL(videoEncodingRequested(QString, QString)), this,
                     SLOT(startEncodingVideo(QString, QString)));
}

/*
 * Called from ActualDetector to start recording. Mat &firstFrame is the frame that caused the positive detection
 */
bool Recorder::startRecording(const TimedMat &firstFrame) {
    qDebug() << "startRecording";
    if (!m_recording) {
        m_firstFrame = firstFrame;
        m_camera->startRecording();
        m_recording = true;
        m_recorderThread = std::make_unique<std::thread>(&Recorder::recordThread, this);
        return true;
    } else {
        qDebug() << "Recorder already recording";
        return false;
    }
}

/*
 * Thread to record a video
 */
void Recorder::recordThread() {
    qInfo() << "++++++++ recorder thread called +++";
    emit recordingStarted();

    m_videoFileExtension = ".yuv";
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss");
    QString filenameTemp = m_resultVideoDirName + "/Capture--" + dateTime + "temp" + m_videoFileExtension;
    QString filenameFinal = m_resultVideoDirName + "/Capture--" + dateTime + ".mp4";

    cv::VideoWriter m_videoWriter(filenameTemp.toStdString(),
                                  0,
                                  1, m_videoResolution);


    qInfo() << ("Video timestamp " + dateTime);

    bool recordCodecIsFinal = false; // todo make this configurable

    if (!m_videoWriter.isOpened()) {
        qInfo() << ("ERROR: Failed to write temporary video " + filenameTemp);
        return;
    }

    QElapsedTimer timer;
    timer.start();
    bool firstFrameWritten = false;
    m_stopRecording = false;

    while (true) {

        auto timedFrame = m_camera->getFrameRecorder();

        if (!firstFrameWritten && m_firstFrame.timestamp < timedFrame.timestamp) {
            m_videoWriter.write(m_firstFrame.frame);
            firstFrameWritten = true;
        }

        if (m_stopRecording && timedFrame.timestamp > m_lastFrameTimestamp) {
            qDebug() << "last frame written";
            break;
        }

        if (timedFrame.frame.empty()) {
            qInfo() << ("ERROR: recorder Failed to read frame from camera");
            continue;
        }

        if (!m_stopRecording && timer.elapsed() > 30000) {
            qInfo() << ("Recording stopped, maximum video length reached");
            m_willSaveVideo = true;
            m_stopRecording = true;
            emit videoRecordingTimeout();
        }

        m_videoWriter.write(timedFrame.frame);

        timedFrame.frame.release();
    }

    qDebug() << "finishing recording";

    m_videoWriter.release();
    m_camera->stopRecording();
    int millisec = timer.elapsed();
    QString videoLength = QString("%1:%2").arg(millisec / 60000, 2, 10, QChar('0'))
            .arg((millisec % 60000) / 1000, 2, 10, QChar('0'));
    qInfo() << ("Video length " + videoLength);

    if (m_willSaveVideo) {
        saveVideoThumbnailImage(m_firstFrame.frame, dateTime);

        if (!recordCodecIsFinal) {
            // Convert raw video to final codec with external encoder
            emit videoEncodingRequested(filenameTemp, filenameFinal);
        } else {
            // Rename temp video to final
            rename(filenameTemp.toLocal8Bit().data(), filenameFinal.toLocal8Bit().data());
            qInfo() << ("Finished recording, saved video");
            emit recordingFinished();
        }

    } else {
        remove(filenameTemp.toLocal8Bit().data());
        qInfo() << ("Finished recording, discarded video");
        emit recordingFinished();
    }
}


void Recorder::saveVideoThumbnailImage(Mat &image, QString dateTime) {
    Mat thumbnail = image.clone();
    cv::resize(thumbnail, thumbnail, m_thumbnailResolution, 0, 0, INTER_CUBIC);
    QString thumbnailFileName = m_resultVideoDirName + "/" + m_thumbnailDirName + "/" + dateTime + m_thumbnailExtension;
    imwrite(thumbnailFileName.toStdString(), thumbnail);
}

/*
 * Called from ActualDetector to stop recording. Specifies if video will be saved
 */
void Recorder::stopRecording(bool willSaveVideo, long lastFrameTimestamp) {
    qDebug() << "stopRecording start";
    if (m_recording) {
        m_willSaveVideo = willSaveVideo;
        m_lastFrameTimestamp = lastFrameTimestamp;
        m_stopRecording = true;
        m_camera->stopRecorderBuffer();
        if (m_recorderThread) {
            m_recorderThread->join();
            m_recorderThread.reset();
        }
        m_recording = false;
    }
    qDebug() << "stopRecording end";
}


/*
 * Called from ActualDetector to pass the rectangle that highlights the detected object and bool isPositive to specify the color of the rectangle
 * Red = positive detection | Blue = negative detection
 */
void Recorder::setRectangle(Rect &r, bool isPositive) {
    m_motionRectangle = r;
    if (isPositive) {
        m_objectRectangleColor = m_objectPositiveColor;
    } else m_objectRectangleColor = m_objectNegativeColor;
}

void Recorder::stopRecordingAndSaveVideo() {
    qDebug() << ("Video recording timeout");
    stopRecording(true, std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
}

void Recorder::startEncodingVideo(const QString &tempVideoFileName, const QString &targetVideoFileName) {
    qDebug() << ("Starting video encoder");
    auto *proc = new QProcess();
    m_encoderProcesses.push_back(proc);
    m_tempVideoFiles.push_back(tempVideoFileName);
    QStringList args;
    args << "-f" << "rawvideo" << "-s" << "1920x1080" << "-r" << "30" << "-pix_fmt" << "yuv420p" << "-i"
         << tempVideoFileName << "-vcodec" << "libx264" << "-profile:v" << "high" << "-level:v" << "4.0" << "-refs"
         << "4" << "-b:v" << "6000k" << targetVideoFileName;
    connect(proc, SIGNAL(finished(int)), this, SLOT(onVideoEncodingFinished()));
    proc->start("ffmpeg", args);
}

void Recorder::onVideoEncodingFinished() {
    qDebug() << ("Video encoder finished");
    QVector<unsigned int> markedForDeletion;
    unsigned int i = 0;

    for (i = 0; i < m_encoderProcesses.size(); i++) {
        QProcess *process = m_encoderProcesses.at(i);
        if (process && (process->state() == QProcess::NotRunning)) {
            markedForDeletion << i;
        }
    }

    QVectorIterator<unsigned int> delIt(markedForDeletion);

    while (delIt.hasNext()) {
        i = delIt.next();

        QProcess *process = m_encoderProcesses.at(i);
        process->deleteLater();
        m_encoderProcesses.erase(m_encoderProcesses.begin() + i);

        QString tempVideoFile = m_tempVideoFiles.at(i);
        remove(tempVideoFile.toStdString().c_str());
        m_tempVideoFiles.erase(m_tempVideoFiles.begin() + i);
    }

    if (m_encoderProcesses.empty()) {
        qDebug() << ("Video encoder(s) finished");
    }
}