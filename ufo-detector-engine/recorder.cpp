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

Recorder::Recorder(Camera *cameraPtr, Config *configPtr) :
    m_camera(cameraPtr), m_config(configPtr)
{
    qDebug() << "Creating recorder";
    const int width = m_config->cameraWidth();
    const int height  = m_config->cameraHeight();
    m_drawRectangles = m_config->resultVideoWithObjectRectangles();
    m_resultDataFile.setFileName(m_config->resultDataFile());
    m_resultVideoDirName = m_config->resultVideoDir();
    m_thumbnailDirName = "thumbnails";
    m_thumbnailExtension = ".jpg";

    QDir dir(QString(m_resultVideoDirName + "/" + m_thumbnailDirName));
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    m_videoFileExtension = ".avi";
    m_objectPositiveColor = Scalar(255, 0, 0);
    m_objectNegativeColor = Scalar(0, 0, 255);
    m_objectRectangleColor = m_objectPositiveColor;
    m_videoResolution = Size(width, height);

    double aspectRatio = (double)width / (double)height;
    m_defaultThumbnailSideLength = 80;
    int thumbnailWidth = m_defaultThumbnailSideLength;
    int thumbnailHeight = qBound(m_defaultThumbnailSideLength / 2,
            (int)(thumbnailWidth / aspectRatio), m_defaultThumbnailSideLength);
    m_thumbnailResolution = Size(thumbnailWidth, thumbnailHeight);

    reloadResultDataFile();
    m_recording = false;
    connect(this, SIGNAL(videoEncodingRequested(QString,QString)), this, SLOT(startEncodingVideo(QString,QString)));
    qDebug() << "Recorder created";
}

/*
 * Called from ActualDetector to start recording. Mat &firstFrame is the frame that caused the positive detection
 */
void Recorder::startRecording(Mat &firstFrame)
{
    if (!m_recording)
    {
        m_firstFrame = firstFrame;
        m_recording = true;
        m_currentFrame = m_camera->getWebcamFrame();
        if (!m_frameUpdateThread)
        {
            m_frameUpdateThread.reset(new std::thread(&Recorder::readFrameThread, this));
        }
        m_recorderThread.reset(new std::thread(&Recorder::recordThread, this));
    }
}

/*
 * Thread to record a video
 */
void Recorder::recordThread(){
    qDebug() << "++++++++recorder thread called+++";
    emit recordingStarted();

    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss");
    QString filenameTemp = m_resultVideoDirName + "/Capture--" + dateTime + "temp" + m_videoFileExtension;
    QString filenameFinal = m_resultVideoDirName + "/Capture--" + dateTime + m_videoFileExtension;
    int recordCodec = m_config->resultVideoCodec();
    bool recordCodecIsFinal = true;
    VideoCodecSupportInfo* codecInfo = m_config->videoCodecSupportInfo();

    // record temporary video directly with OpenCV if it supports the final codec
    if (!codecInfo->isOpencvSupported(recordCodec))
    {
        // no support in OpenCV -> record with raw video codec
        recordCodec = codecInfo->stringToFourcc(codecInfo->rawVideoCodecStr());
        if (codecInfo->isEncoderSupported(m_config->resultVideoCodec()))
        {
            // encode only when encoder supports codec, otherwise video will be left raw video
            recordCodecIsFinal = false;
        }
    }
    m_videoWriter.open(filenameTemp.toStdString(), recordCodec, OUTPUT_FPS, m_videoResolution, true);

    qDebug() << "Video timestamp" << dateTime;

    if (!m_videoWriter.isOpened())
    {
        qDebug() << "ERROR: Failed to write temporary video" << filenameTemp;
        return;
    }

    prevTime = std::chrono::high_resolution_clock::now();
    currentTime = prevTime + std::chrono::high_resolution_clock::duration(40000000);
    auto difference = currentTime - prevTime;

    QTime timer;
    timer.start();
    if (m_firstFrame.data)
    {
        m_videoWriter.write(m_firstFrame);
    }

    while(m_recording)
    {
        while (difference < frame_period{ 1 })
        {
            currentTime = std::chrono::high_resolution_clock::now();
            difference = currentTime - prevTime;
        }
        m_currentFrameMutex.lock();
        if (m_currentFrame.data)
        {
            m_videoWriter.write(m_currentFrame);
        }
        m_currentFrameMutex.unlock();

        prevTime = std::chrono::time_point_cast<hr_duration>(prevTime + frame_period{ 1 });
        difference = currentTime - prevTime;
        std::this_thread::yield();
    }

    m_videoWriter.release();
    int millisec = timer.elapsed();
    QString videoLength = QString("%1:%2").arg( millisec / 60000, 2, 10, QChar('0'))
            .arg((millisec % 60000) / 1000, 2, 10, QChar('0'));
    qDebug() << "Video length" << videoLength;

    if(m_willSaveVideo)
    {
        saveResultData(dateTime, videoLength);
        if(!recordCodecIsFinal)
        {
            // Convert raw video to final codec with external encoder
            emit videoEncodingRequested(filenameTemp, filenameFinal);
        }
        else
        {
            // Rename temp video to final
            rename(filenameTemp.toLocal8Bit().data(), filenameFinal.toLocal8Bit().data());
            qDebug() << "Finished recording, saved video";
            emit recordingFinished();
        }
    }
    else
    {
        remove(filenameTemp.toLocal8Bit().data());
        qDebug() << "Finished recording, discarded video";
        emit recordingFinished();
    }
}

void Recorder::startEncodingVideo(QString tempVideoFileName, QString targetVideoFileName)
{
    QProcess* proc = new QProcess();
    m_encoderProcesses.push_back(proc);
    m_tempVideoFiles.push_back(tempVideoFileName);
    QStringList args;
    int codec = m_config->resultVideoCodec();
    QString codecStr = m_config->videoCodecSupportInfo()->fourccToEncoderString(codec);
    // these video encoder parameters are ok only for ffmpeg and avconv (which are more or less compatible)
    args << "-i" << tempVideoFileName << "-vcodec" << codecStr << targetVideoFileName;
    connect(proc, SIGNAL(finished(int)), this, SLOT(onVideoEncodingFinished()));
    proc->start(m_config->videoEncoderLocation(), args);
}

/*
 * Called when the ffmpeg encoding finishes.  Removes the temporary video file
 * If no process is running emit recordingFinished()
 */
void Recorder::onVideoEncodingFinished()
{
    QVector<unsigned int> markedForDeletion;
    unsigned int i = 0;

    for (i = 0; i < m_encoderProcesses.size(); i++)
    {
        QProcess* process = m_encoderProcesses.at(i);
        if (process && (process->state() == QProcess::NotRunning))
        {
            markedForDeletion << i;
        }
    }

    QVectorIterator<unsigned int> delIt(markedForDeletion);

    while (delIt.hasNext())
    {
        i = delIt.next();

        QProcess* process = m_encoderProcesses.at(i);
        process->deleteLater();
        m_encoderProcesses.erase(m_encoderProcesses.begin() + i);

        QString tempVideoFile = m_tempVideoFiles.at(i);
        remove(tempVideoFile.toStdString().c_str());
        m_tempVideoFiles.erase(m_tempVideoFiles.begin() + i);
    }

    if(m_encoderProcesses.size() == 0)
    {
        qDebug() << "Video encoder(s) finished";
        emit recordingFinished();
    }
}

/*
 * Reads frame from Camera while video is recording. Adds rectangle from setRectangle() to video
 */
void Recorder::readFrameThread()
{
    Mat temp;
    Rect oldRectangle;
    while(m_recording)
    {
        temp = m_camera->getWebcamFrame();
        if (m_drawRectangles && (m_motionRectangle != oldRectangle))
        {
            rectangle(temp, m_motionRectangle, m_objectRectangleColor);
            oldRectangle=m_motionRectangle;
        }
        m_currentFrameMutex.lock();
        temp.copyTo(m_currentFrame);
        m_currentFrameMutex.unlock();
        std::this_thread::yield();
    }
}

void Recorder::saveResultData(QString dateTime, QString videoLength)
{
    cv::resize(m_firstFrame, m_firstFrame, m_thumbnailResolution, 0, 0, INTER_CUBIC);
    QString thumbnail = m_resultVideoDirName + "/" + m_thumbnailDirName + "/" + dateTime + m_thumbnailExtension;
    imwrite(thumbnail.toStdString(), m_firstFrame);

    /// @todo create root element here if it doesn't exist
    QDomElement node = m_resultDataDomDocument.createElement("Video");
    node.setAttribute("Pathname", m_resultVideoDirName);
    node.setAttribute("DateTime", dateTime);
    node.setAttribute("Length", videoLength);
    m_resultDataRootElement.appendChild(node);

    if(!m_resultDataFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Recorder: failed to open result data file" << m_resultDataFile.fileName();
        return;
    }
    else
    {
        QTextStream stream(&m_resultDataFile);
        stream.setCodec("UTF-8");
        stream << m_resultDataDomDocument.toString();
        if (stream.status() != QTextStream::Ok)
        {
            qDebug() << "Problem writing to result data file" << m_resultDataFile.fileName();
        }
        m_resultDataFile.flush();
        m_resultDataFile.close();
    }
    emit resultDataSaved(m_resultVideoDirName, dateTime, videoLength);
}

/*
 * Slot to reload the log file and get the root element. Called from MainWindow when a VideoWidget was removed
 */
void Recorder::reloadResultDataFile()
{
    if(!m_resultDataFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Recorder: failed to open result data file" << m_resultDataFile.fileName();
    }
    else
    {
        if(!m_resultDataDomDocument.setContent(&m_resultDataFile))
        {
            qDebug() << "Recorder: failed to reload result data file" << m_resultDataFile.fileName();
        }
        else
        {
            m_resultDataRootElement = m_resultDataDomDocument.firstChildElement();
        }
        m_resultDataFile.close();
    }
}

/*
 * Called from ActualDetector to stop recording. Specifies if video will be saved
 */
void Recorder::stopRecording(bool willSaveVideo)
{
    if(m_recorderThread)
    {

        m_willSaveVideo=willSaveVideo;
        m_recording = false;
        m_recorderThread->join(); m_recorderThread.reset();
        m_frameUpdateThread->join(); m_frameUpdateThread.reset();
    }
}

/*
 * Called from ActualDetector to pass the rectangle that highlights the detected object and bool isPositive to specify the color of the rectangle
 * Red = positive detection | Blue = negative detection
 */
void Recorder::setRectangle(Rect &r, bool isPositive)
{
    m_motionRectangle=r;
    if(isPositive)
    {
        m_objectRectangleColor = m_objectPositiveColor;
    }
    else  m_objectRectangleColor = m_objectNegativeColor;
}

