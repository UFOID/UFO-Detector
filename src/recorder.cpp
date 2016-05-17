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
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTime>
#include <QTextStream>
#include <QFile>
#include "camera.h"
#include "actualdetector.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

#define OUTPUT_FPS 25

using namespace cv;
using namespace std;
using frame_period = std::chrono::duration<long long, std::ratio<1, OUTPUT_FPS>>;
using hr_duration = std::chrono::high_resolution_clock::duration;


Recorder::Recorder(ActualDetector *parent, Camera *cameraPtr, Config *configPtr) :
    QObject(parent), camPtr(cameraPtr), m_config(configPtr)
{
    cout << "begin recorder " << endl;
    const int WIDTH = m_config->cameraWidth();
    const int HEIGHT  = m_config->cameraHeight();
    const QString recordingPath = m_config->resultVideoDir();
    withRectangle = m_config->resultVideoWithObjectRectangles();
    pathDirectory = recordingPath.toLatin1().toStdString();
    codecSetting = m_config->resultVideoCodec();

    QDir dir(QString(recordingPath+"/thumbnails/"));
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    ext=".avi";
    color = Scalar(255,0,0);
    resolutionRecording = Size(WIDTH,HEIGHT);
    int aspectRatio = (double)WIDTH/HEIGHT * 10000;
    switch (aspectRatio)
    {
    case 12222:
        resolutionThumbnail = Size((640/10) + 15, (524/10) + 15);
        break;
    case 13333:
        resolutionThumbnail = Size((640/10) + 15, (480/10) + 15);
        break;
    case 15000:
        resolutionThumbnail = Size((640/10) + 15, (320/10) + 15);
        break;
    case 17777:
        resolutionThumbnail = Size((640/10) + 15, (360/10) + 15);
        break;
    default:
        qDebug() << "Recorder::Recorder(): got unknown webcam aspect ratio, problems ahead";
        break;
    }

    if (codecSetting == 2)
    {
        codec = CV_FOURCC('L', 'A', 'G', 'S');
    }
    else if (codecSetting == 0 || codecSetting == 1)
    {
        codec = DEFAULT_CODEC;
    }

    reloadResultDataFile();
    recording = false;
    connect(this,SIGNAL(finishedRec(QString,QString)),this,SLOT(finishedRecording(QString,QString)));
    cout << "recorder constructed" << endl;
}

/*
 * Called from ActualDetector to start recording. Mat &f is the frame that caused the positive detection
 */
void Recorder::startRecording(Mat &f)
{
    if (!recording)
    {
        recording=true;
        frameToRecord = camPtr->getWebcamFrame();
        if(withRectangle)
        {
            if (!frameUpdateThread) frameUpdateThread.reset(new std::thread(&Recorder::readFrameThread, this));
        }
        else if (!frameUpdateThread) frameUpdateThread.reset(new std::thread(&Recorder::readFrameThreadWithoutRect, this));
        firstFrame=f;
        recThread.reset(new std::thread(&Recorder::recordThread, this));
    }
}

/*
 * Thread to record a video
 */
void Recorder::recordThread(){
    cout << "++++++++recorder thread called+++" << endl;
    emit recordingStarted();

    string dateTime=QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss").toStdString();
    string filename= pathDirectory+"/Capture--"+dateTime+"temp"+ext;
    string filenameFinal= pathDirectory+"/Capture--"+dateTime+ext;
    theVideoWriter.open(filename,codec, OUTPUT_FPS, resolutionRecording, true);

    if (!theVideoWriter.isOpened())
    {
        cout << "ERROR: Failed to write the video" << filename << endl;
        return;
    }

    prev = std::chrono::high_resolution_clock::now();
    current = prev + std::chrono::high_resolution_clock::duration(40000000);
    auto difference = current-prev;

    QTime timer;
    timer.start();
    if (firstFrame.data)
    {
        theVideoWriter.write(firstFrame);
    }

    while(recording)
    {
        while (difference < frame_period{ 1 })
        {
            current = std::chrono::high_resolution_clock::now();
            difference = current - prev;
        }

        if (frameToRecord.data)
        {
            theVideoWriter.write(frameToRecord);
        }

        prev = std::chrono::time_point_cast<hr_duration>(prev + frame_period{ 1 });
        difference = current - prev;
    }

    theVideoWriter.release();
    int millisec = timer.elapsed();
    QString videoLength = QString("%1:%2").arg( millisec / 60000        , 2, 10, QChar('0'))
            .arg((millisec % 60000) / 1000, 2, 10, QChar('0'));
    cout << videoLength.toStdString() << endl;

    if(willBeSaved)
    {
        saveLog(dateTime,videoLength);
        if(codecSetting==1)
        {	//Convert raw video to FFV1
            emit finishedRec(filename.c_str(),filenameFinal.c_str());
        }
        else
        {	//Rename temp video to final
            rename(filename.c_str(),filenameFinal.c_str());
            emit recordingStopped();
        }
    }
    else
    {
        remove(filename.c_str());
        emit recordingStopped();
    }
    cout << "finished recording" << endl;

}

/*
 * Creates a new QProcess that encodes the raw video to FFV1 with ffmpeg
 */
void Recorder::finishedRecording(QString picDir, QString filename)
{
    QProcess* proc = new QProcess();
    vecProcess.push_back(proc);
    vecString.push_back(picDir);
    // these video encoder parameters are ok only for ffmpeg and avconv (which are more or less compatible)
    proc->start(m_config->videoEncoderLocation(), QStringList() <<"-i"<< picDir << "-vcodec"<< "ffv1" << filename);
    connect(proc,SIGNAL(finished(int)),this,SLOT(finishedProcess()));
}

/*
 * Called when the ffmpeg encoding finishes.  Removes the temporary video file
 * If no process is running emit recordingStopped()
 */
void Recorder::finishedProcess()
{
    for (unsigned int i=0; i<vecProcess.size();i++)
    {
        if(vecProcess.at(i)->state()==QProcess::NotRunning)
        {
            qDebug()<<" pos: " + QString::number(i);
            delete vecProcess.at(i);
            vecProcess.erase(vecProcess.begin() + i);

            QString folderToRemove=vecString.at(i);

            qDebug() << folderToRemove;
            remove(folderToRemove.toStdString().c_str());
            vecString.erase(vecString.begin() + i);

            if(vecProcess.size()==0)
            {
                emit recordingStopped();
            }
            break;
        }
    }
}

/*
 * Reads frame from Camera while video is recording. Adds rectangle from setRectangle() to video
 */
void Recorder::readFrameThread()
{
    Mat temp;
    Rect oldRectangle;
    while(recording)
    {
        temp = camPtr->getWebcamFrame();
        if(motionRectangle!=oldRectangle)
        {
            rectangle(temp,motionRectangle,color);
            oldRectangle=motionRectangle;
        }
        temp.copyTo(frameToRecord);
    }
}

/*
 * Reads frame from Camera while video is recording
 */
void Recorder::readFrameThreadWithoutRect()
{
    Mat temp;
    while(recording)
    {
        temp = camPtr->getWebcamFrame();
        temp.copyTo(frameToRecord);
    }
}

/*
 * Save information about the video to log file. Emits signal that will add new VideoWidget to MainWindow
 */
void Recorder::saveLog(string dateTime, QString videoLength)
{
    cv::resize(firstFrame,firstFrame, resolutionThumbnail,0, 0, INTER_CUBIC);
    String thumbnail = pathDirectory+"/thumbnails/"+dateTime+".jpg";
    imwrite(thumbnail, firstFrame );

    QDomElement node = documentXML.createElement("Video");
    node.setAttribute("Pathname", QString::fromLatin1(pathDirectory.c_str()));
    node.setAttribute("DateTime", dateTime.c_str());
    node.setAttribute("Length", videoLength);
    rootXML.appendChild(node);

    if(!resultDataFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        cout << "fail open" << endl;
        return;
    }
    else
    {
        QTextStream stream(&resultDataFile);
        stream.setCodec("UTF-8");
        stream << documentXML.toString();
        resultDataFile.close();
    }
    emit updateListWidget(QString::fromLatin1(pathDirectory.c_str()),dateTime.c_str(),videoLength);

}

/*
 * Slot to reload the log file and get the root element. Called from MainWindow when a VideoWidget was removed
 */
void Recorder::reloadResultDataFile()
{
    resultDataFile.setFileName(m_config->resultDataFile());
    if(!resultDataFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "failed reading the result data file" << endl;
    }
    else
    {
        if(!documentXML.setContent(&resultDataFile))
        {
            qDebug() << "failed to load log document recorder" << endl;
        }
        else
        {
            rootXML=documentXML.firstChildElement();
            resultDataFile.close();
        }
    }
}

/*
 * Called from ActualDetector to stop recording. Bool b specifies if the video will be saved or deleted
 */
void Recorder::stopRecording(bool willSaveVideo)
{
    if(recThread)
    {

        willBeSaved=willSaveVideo;
        recording = false;
        recThread->join(); recThread.reset();
        frameUpdateThread->join(); frameUpdateThread.reset();
    }
}

/*
 * Called from ActualDetector to pass the rectangle that highlights the detected object and bool isRed to specify the color of the rectangle
 * Red = positive detection | Blue = negative detection
 */
void Recorder::setRectangle(Rect &r, bool isRed)
{
    motionRectangle=r;
    if(isRed)
    {
        color = Scalar(0,0,255);
    }
    else  color = Scalar(255,0,0);
}

