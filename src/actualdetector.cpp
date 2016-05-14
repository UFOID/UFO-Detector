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

#include "actualdetector.h"
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <chrono>
#include <stdio.h>
#include "camera.h"
#include "mainwindow.h"
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QtXml>
#include "Ctracker.h"
#include "Detector.h"

using namespace cv;

/*
 * Main Detector class
 */
ActualDetector::ActualDetector(MainWindow *parent, Camera *cameraPtr, Config *configPtr) :
    QObject(parent), theRecorder(this, cameraPtr, configPtr), camPtr(cameraPtr)
{
    m_config = configPtr;
    const QString DETECTION_AREA_FILE = m_config->detectionAreaFile();
    const QString IMAGEPATH = m_config->resultImageDir() + "/";
    willSaveImages = m_config->saveResultImages();
    WIDTH = m_config->cameraWidth();
    HEIGHT = m_config->cameraHeight();
    willRecordWithRect = m_config->resultVideoWithObjectRectangles();
    minPositiveRequired = m_config->minPositiveDetections();

    detectionAreaFile = DETECTION_AREA_FILE.toStdString();
    pathname = IMAGEPATH.toStdString();
    kernel_ero = getStructuringElement(MORPH_RECT, Size(1,1));

    std::cout << "actualdetector constructed" <<endl;
}

/*
 * Initialize the Detector
 */
bool ActualDetector::initialize()
{

    prev_frame = camPtr->getWebcamFrame();
    result = prev_frame;
    current_frame = camPtr->getWebcamFrame();
    next_frame = camPtr->getWebcamFrame();
    cvtColor(current_frame, current_frame, CV_RGB2GRAY);
    cvtColor(prev_frame, prev_frame, CV_RGB2GRAY);
    cvtColor(next_frame, next_frame, CV_RGB2GRAY);

    minAmountOfMotion = 2;
    max_deviation = 20;
    imageCount = 0;
    ext =".jpg";

    bool success = parseDetectionAreaFile(detectionAreaFile, region);

    willDisplayImage= qobject_cast <MainWindow*>(parent())->getCheckboxState();
    willParseRectangle=false;
    if(willSaveImages)
	{
        string dateTime=QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss").toStdString();
        QDir dir = QDir::root();
        dir.mkpath(QString::fromStdString(pathname + dateTime));
        fileDir = (QString::fromStdString(pathname + dateTime +"/"+"image")).toLatin1().toStdString();
        pathnameThresh = fileDir + "Thresh";
    }

    std::cout << "initialized Detector" << endl;
    this_thread::sleep_for(std::chrono::seconds(1));
    isInNightMode = false;    
    startedRecording = false;

    rect = Rect(Point(0,0),Point(WIDTH,HEIGHT));
    treshImg = result.clone();
    treshImg.setTo(Scalar(0,0,0));
    return success;
}

/*
 * The detection thread
 */
void ActualDetector::detectingThread()
{
    qDebug() << "started normal thread";
    Mat tempImage;
    int posCounter = 0;
    int negAndNoMotionCounter = 0;
    int counterNoMotion = 0;
    int counterBlackDetecor = 0;
    int counterLight = 0;
    bool isPositiveRectangle;

    CTracker tracker(0.2,0.5,60.0,15,15);
    CDetector* detector=new CDetector(current_frame);
    vector<Point2d> centers;
    Scalar Colors[]={Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255),Scalar(255,255,0),Scalar(0,255,255),Scalar(255,0,255),Scalar(255,127,255),Scalar(127,0,255),Scalar(127,0,127)};
    pair < vector<Point2d>,vector<Rect> > centerAndRectPair;


    if (!birds_cascade.load(m_config->birdClassifierTrainingFile().toStdString()))
	{
        QString output_text = "ERROR could not load cascade classifier";
        emit broadcastOutputText(output_text);
	}

    while (isActive)
	{
            prev_frame = current_frame;
            current_frame = next_frame;
            tempImage = camPtr->getWebcamFrame();

            next_frame = tempImage.clone();
            result = next_frame;
            cvtColor(next_frame, next_frame, CV_RGB2GRAY);

            absdiff(prev_frame, next_frame, d1);
            absdiff(current_frame, next_frame, d2);
            bitwise_and(d1, d2, motion);
            threshold(motion, motion, thresholdLevel, 255, CV_THRESH_BINARY);

            erode(motion, motion, kernel_ero);

            numberOfChanges = detectMotion(motion, result, result_cropped, region, max_deviation);

            if(numberOfChanges>=minAmountOfMotion) 
			{
                centerAndRectPair = detector->Detect(treshImg,rect);
                centers = centerAndRectPair.first;
                detectorRectVec = centerAndRectPair.second;
                counterNoMotion=0;
                if(centers.size()>0)
				{
                    tracker.Update(centers);
                }
                //loop through detected objects
                if (detectorRectVec.size()<10)
				{
                    isPositiveRectangle=false;
                    for ( unsigned int i=0;i<detectorRectVec.size();i++)
					{
                        Rect croppedRectangle = detectorRectVec[i];
                        Mat temp = result(croppedRectangle);
                        Mat croppedImage = temp.clone();
                        //+++check if there was light in object
                        if(lightDetection(croppedRectangle,croppedImage))
						{
                            //object was bright
                            if (!isInNightMode && checkIfBird())
							{
                                tracker.tracks[i]->birdCounter++;
                                tracker.tracks[i]->negCounter++;
                            }
                            else 
							{//+++ not in night mode or was not a bird*/
                                counterLight++;
                                if(counterLight>2)counterBlackDetecor=0;
                                if(counterBlackDetecor<5)
								{
                                    if(!startedRecording)
									{                                        
                                        Mat tempImg = result.clone();
                                        rectangle(tempImg,croppedRectangle,Scalar(255,0,0),1);
                                        theRecorder.setup(tempImg);
                                        if(willRecordWithRect) willParseRectangle=true;
                                        startedRecording=true;
	                                    auto output_text = "Positive detection - Starting video recording";
                                        emit broadcastOutputText(output_text);
                                    }

                                    if(willParseRectangle)
									{
										isPositiveRectangle=true;
									}
                                    negAndNoMotionCounter=0;
                                    posCounter++;
                                    tracker.tracks[i]->posCounter++;
                                    emit positiveMessage();

                                    if(willSaveImages)
									{
                                        saveImg(fileDir, croppedImage); imageCount++;
                                        //saveImg(pathnameThresh, treshImg);
                                    }									
                                }
                            }
                        }

                        else { //+++motion has black pixel
                            counterBlackDetecor++;
                            counterLight=0;
                            tracker.tracks[i]->negCounter++;
                            /*if(willSaveImages){
                                saveImg(fileDir, result_cropped); imageCount++;
                                saveImg(pathnameThresh, treshImg); imageCount++;
                            }*/
                            if (startedRecording)
							{
                                negAndNoMotionCounter++;
                            }
                            emit negativeMessage();
                        }
                    }
                    if(willParseRectangle)
					{
                        theRecorder.setRectangle(rect,isPositiveRectangle);
                    }

                }
            }
            else
			{ //+++no motion detected
                counterLight=0;
                counterBlackDetecor=0;
                counterNoMotion++;
                if (startedRecording)
				{
                    negAndNoMotionCounter++;
                }
                tracker.updateEmpty();
                centers.clear();
                detectorRectVec.clear();
                if ((startedRecording && counterNoMotion > 150) || (negAndNoMotionCounter > 700))
				{ //+++no motion for 150 checks after recording started
                    if(tracker.wasBird)
					{
	                    auto output_text = "One object identified as a bird";
						emit broadcastOutputText(output_text);
                    }
                    if(!tracker.removedTrackWithPositive)
					{ //+++all detected objects had more negative than positive detections or was a bird
                        theRecorder.stopRecording(false);
	                    auto output_text = "Finished Recording - All objects found negative: Removed Video";
						emit broadcastOutputText(output_text);

                    }
                    else
					{//+++one object had more positive than negative detections
                        if(posCounter>=minPositiveRequired)
						{
                            theRecorder.stopRecording(true);
                            auto output_text = "Finished Recording - At least one object found positive: Saved Video";
							emit broadcastOutputText(output_text);
                        }
                        else
						{
                            theRecorder.stopRecording(false);
                            auto output_text = "Finished Recording - Minimum required positive detections not met: Removed Video";
							emit broadcastOutputText(output_text);
                        }
                    }                    
                    tracker.removedTrackWithPositive=false;
                    tracker.wasBird=false;
                    negAndNoMotionCounter=0;
                    posCounter=0;
                    willParseRectangle=false;
                    startedRecording=false;
                }
            }



            if (willDisplayImage)
			{
                for(unsigned int i=0; i<centers.size(); i++)
				{
                    //rectangle(result,detectorRectVec[i],color,1);
                    circle(result,centers[i],3,Scalar(0,255,0),1,CV_AA);
                    // stringstream ss;
                    // char str[256] = "";                    
                    // snprintf(str, sizeof(str), "%zu", tracker.tracks[i]->track_id);
                    // ss << str << " P: " << tracker.tracks[i]->posCounter << " N: " << tracker.tracks[i]->negCounter;
                    // putText(result,ss.str(),centers[i],CV_FONT_HERSHEY_PLAIN,2, CV_RGB(250,0,0));
                }
                if(centers.size()>0)
				{
                    for(unsigned int i=0;i<tracker.tracks.size();i++)
					{
                        if(tracker.tracks[i]->trace.size()>1)
						{
                            for(unsigned int j=0;j<tracker.tracks[i]->trace.size()-1;j++)
							{
                                line(result,tracker.tracks[i]->trace[j],tracker.tracks[i]->trace[j+1],Colors[tracker.tracks[i]->track_id%9],2,CV_AA);
                            }
                        }
                    }
                }

                /// @todo use actual view size, not hardcoded value
                cv::resize(result,result, Size(640,360) ,0, 0, INTER_CUBIC);
                cv::cvtColor(result, result, CV_BGR2RGB);
                QImage qimOriginal((uchar*)result.data, result.cols, result.rows, result.step, QImage::Format_RGB888);
                emit updatePixmap(qimOriginal);
            }
    }
    delete detector;
}



/*
 * Check if there was motion between frames. Return the AmountOfMotion detected
 */
inline int ActualDetector::detectMotion(const Mat & motion, Mat & result, Mat & result_cropped,vector<Point> & region,int max_deviation)
{
	// calculate the standard deviation
	Scalar mean, stddev;
	meanStdDev(motion, mean, stddev);
	// if not to much changes then the motion is real
	if(stddev[0] < max_deviation)
	{
		int number_of_changes = 0;
		int min_x = motion.cols, max_x = 0;
		int min_y = motion.rows, max_y = 0;
		// loop over image and detect changes
		int x, y, size = region.size();

		for(int i = 0; i < size; i++)
		{ // loop over region
			x = region[i].x;
			y = region[i].y;
			if(static_cast<int>(motion.at<uchar>(y,x)) == 255)
			{
				number_of_changes++;
				if(min_x>x) min_x = x;
				if(max_x<x) max_x = x;
				if(min_y>y) min_y = y;
				if(max_y<y) max_y = y;
			}
		}
		if(number_of_changes)
		{
			//check if not out of bounds
			if(min_x-10 > 0) min_x -= 10;
			if(min_y-10 > 0) min_y -= 10;
			if(max_x+10 < result.cols-1) max_x += 10;
			if(max_y+10 < result.rows-1) max_y += 10;

			Point x(min_x,min_y);
			Point y(max_x,max_y);
			rect = Rect(x,y);
			Mat croppedThresh = motion(rect);
			croppedThresh.copyTo(treshImg);
			Mat cropped = result(rect);
			cropped.copyTo(result_cropped);

		}
		else
		{
			rect = Rect(Point(0,0),Point(WIDTH,HEIGHT));
			treshImg = result.clone();
			treshImg.setTo(Scalar(0,0,0));
		}
		return number_of_changes;
	}
	return 0;
}

/*
 * Check if an object is bright. I.e. object has more bright pixels than dark pixels
 */
bool ActualDetector::lightDetection(Rect &rectangle, Mat &croppedImage)
{
	bool objectHasLight=false;

	vector<cv::Point> detectionRegion;

	int lightCounter=0;
	int blackCounter=0;
	Mat croppedImageThreshTemp, croppedImageThresh;
	croppedImageThreshTemp = motion(rectangle);
	croppedImageThreshTemp.copyTo(croppedImageThresh);
	cvtColor(croppedImage, croppedImageGray , CV_RGB2GRAY);

	int light=0;
	int totalLight=0;
	bool wasDark=false;

	for(int y = 0; y < croppedImage.rows; y++)
	{
		for(int x = 0; x < croppedImage.cols; x++)
		{
			light+=static_cast<int>(croppedImageGray.at<uchar>(y,x));
			if(static_cast<int>(croppedImageThresh.at<uchar>(y,x)) == 255)
			{
				Point2f p(x,y);
				detectionRegion.push_back(p);
			}
		}
	}

	totalLight=light/(croppedImage.cols*croppedImage.rows);
	pair<int,int> minAndMaxLight = checkBrightness(totalLight);

	int x, y, size = detectionRegion.size();
	for(int j = 0; j < size; j++)
	{
		x = detectionRegion[j].x;
		y = detectionRegion[j].y;
		if(static_cast<int>(croppedImageGray.at<uchar>(y,x)) > minAndMaxLight.first)
		{
			lightCounter++;
		}
		if(static_cast<int>(croppedImageGray.at<uchar>(y,x)) < minAndMaxLight.second)
		{
			blackCounter++;
		}
	}

	if(totalLight<26)
	{  //Activate night mode for object
		wasDark=true;
	}

	if(wasDark)
	{
		if(lightCounter>1)
		{
			objectHasLight=true;
		}
	}
	else
	{
		if (lightCounter>=blackCounter && size>0)
		{
			objectHasLight=true;
		}
	}

	return objectHasLight;
}

/*
 * For parameter totalLight return pair that contains the min values for light (first)
 * and min values for black (second)
 */
pair<int,int> ActualDetector::checkBrightness(int totalLight)
{
    //first int = minLight
    //second int = minBlack
    if (totalLight<10){
        return make_pair(50,30);
    }
    else if (totalLight<70){
        return make_pair(90,75);
    }
    else if (totalLight<100){
        return make_pair(110,85);
    }
    else if (totalLight<130){
        return make_pair(150,130);
    }
    else if (totalLight<150){
        return make_pair(160,145);
    }
    else if (totalLight<180){
        return make_pair(190,170);
    }
    else if (totalLight<210){
        return make_pair(215,205);
    }
    else if (totalLight<230){
        return make_pair(235,225);
    }
    else if (totalLight<241){
        return make_pair(243,235);
    }
    else {
        return make_pair(250,240);
    }

}

/*
 * Thread that check if it is night very 300 seconds (5mins)
 * If total brightness is less than 100 it is night. In that case we temporary stop the detection process
 * to exclude any area from the detection area which is bright (i.e. the moon and stars) in order to
 * ignore any image noise around that area
 */
void ActualDetector::checkIfNight()
{

	bool isRunning=true;    
	vector<Point> regionBackup=region;
	vector<Point> regionNew;
	bool changedRegion=false;
	int timerSeconds=300;

	while(isRunning)
	{
		//get average brightness of region
		int total=0;
		long light=0;
		Mat temp;
		temp = camPtr->getWebcamFrame();
		Mat frame = temp.clone();
		cvtColor(frame, frame , CV_RGB2GRAY);

		for(unsigned int i = 0; i<region.size(); i++)
		{
			int x = region[i].x;
			int y = region[i].y;
			light+=static_cast<int>(frame.at<uchar>(y,x));
		}
		qDebug() << region.size();

		total = light/region.size();
		qDebug() << "total light " << total;

		if (total<100)
		{
			stopOnlyDetecting();
			region=regionBackup;
			isInNightMode=true;

			vector<Rect> constants = getConstantRecs(total);
			if(constants.size()<=4 && constants.size()>0)
			{
				Mat imageBinary(frame.rows,frame.cols,CV_THRESH_BINARY, Scalar(0,0,0));
				//draw white pixel for everything inside rectangles
				for(std::vector<Rect>::iterator it = constants.begin(); it != constants.end(); ++it)
				{
					Rect rectangleArea = *it;
					if(rectangleArea.width<140 && rectangleArea.height<140)
					{
						rectangle(imageBinary,rectangleArea,Scalar(255,255,255),-1);
					}
				}

				//find rectangle coordinates in region and remove them
				regionNew=region;
				vector<Point>::iterator it = regionNew.begin();
				for ( ; it != regionNew.end(); ) 
				{
					if (static_cast<int>(imageBinary.at<uchar>(*it)) != 0) 
					{
						it = regionNew.erase(it);
					} 
					else ++it;
				}

				auto output_text = QString::number(constants.size()) + " area(s) being ignored in order to filter the moon and stars";
				emit broadcastOutputText(output_text);
				changedRegion=true;

			}
			if (!pThread)
			{
				if(changedRegion)
				{
					region=regionNew;
					changedRegion=false;
				}
				regionNew.clear();
				isActive=true;
				pThread.reset(new std::thread(&ActualDetector::detectingThread, this));
			}

		}
		else isInNightMode=false;


		//timer
		int counter=0;
		bool isSleeping=true;
		while(isSleeping)
		{
			if(isActive && counter<timerSeconds)
			{
				this_thread::sleep_for(chrono::seconds(1));
				counter++;
			}
			else if(isActive && counter==timerSeconds)
			{
				isSleeping=false;
				qDebug() << "re run loop and timer";
			}
			else if(!isActive)
			{
				isSleeping=false;
				isRunning=false;
			}
		}
	}
}

/*
 * Called from checkIfNight to temporary stop detecting
 */
void ActualDetector::stopOnlyDetecting()
{
    if(startedRecording)
	{
        qDebug() << "wait till stopped recording";
        this_thread::sleep_for(chrono::seconds(1));
        stopOnlyDetecting();
    }
    else
	{
        if (pThread)
		{
            isActive=false;
            pThread->join();
			pThread.reset();
        }
    }
}

/*
 * Check if single (!) object is a bird
 * TODO Improve to only look for single objects
 */
bool ActualDetector::checkIfBird()
{
    std::vector<Rect> birds;
    birds_cascade.detectMultiScale( croppedImageGray, birds, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(40, 40) );

    if(birds.size()>0)
	{
        return true;
    }
    else return false;
}

/*
 * Get vector with the Rect of all constant bright objects
 */
std::vector<Rect> ActualDetector::getConstantRecs(int totalLight)
{
    vector<Rect> rectVec;
    Mat image = camPtr->getWebcamFrame();
    Mat imageGray;
    cvtColor(image, imageGray , CV_RGB2GRAY);

    int y, x, size;
    size=region.size();
    int minLight = checkBrightness(totalLight).first;

    //find bright pixels in webcam frame and paint pixels in binary image
    Mat imageBinary(image.rows,image.cols,CV_THRESH_BINARY, Scalar(0,0,0));
    for(int i = 0; i < size; i++)
	{
        x = region[i].x;
        y = region[i].y;
        if(static_cast<int>(imageGray.at<uchar>(y,x)) > minLight+10)
		{
            rectangle(imageBinary,Point(x,y),Point(x,y),Scalar(255,255,255),-1);
        }
    }

    //find contours in binary image
    dilate(imageBinary, imageBinary, getStructuringElement(MORPH_RECT, Size(10,10)));
    Mat temp;
    imageBinary.copyTo(temp);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
	
    if (contours.size()<20)
	{
        vector<vector<Point>> contours_poly( contours.size() );
        vector<Rect> boundRect( contours.size() );
        for( unsigned int i = 0; i < contours.size(); i++ )
		{
            approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        }

        for( unsigned int i = 0; i< contours.size(); i++ )
		{
            Rect finalRect = enlargeROI(imageBinary,boundRect[i],15);
            Rect objectRect(finalRect.tl(),finalRect.br());
            rectangle(imageBinary,objectRect,Scalar(255,255,255),1);
            rectVec.push_back(objectRect);
        }
    }

    return rectVec;

}

/*
 * Read the detection area file
 */
bool ActualDetector::parseDetectionAreaFile(string file_region, vector<Point> &region)
{
    region.clear();

    QFile fileXML(file_region.c_str());
    QDomDocument doc;
    QDomElement root;
    int size = m_config->detectionAreaSize();
    int result = 0;

    if(!fileXML.open(QIODevice::ReadOnly | QIODevice::Text))
	{
        qDebug() << "fail reading the file actualldetector" << endl;
        emit errorReadingDetectionAreaFile();
        return false;
    }
    else
	{
        if(!doc.setContent(&fileXML))
		{
            qDebug() << "Failed to read the element";
            emit errorReadingDetectionAreaFile();
            return false;
        }
        else
		{
            root = doc.firstChildElement();
            QDomNode node = root.firstChild();
            int x, y;
            int count = 0;
            while( !node.isNull())
			{
                if( node.isElement())
				{
                    QDomElement element = node.toElement();
                    x = element.attribute("x").toInt();
                    y = element.attribute("y").toInt();					
                    if(y>HEIGHT || x>WIDTH)
					{
                        qDebug() << y;
                        qDebug() << x;
						auto output_text = "ERROR: Selected area of detection does not match camera resolution. Re-select area of detection";
						emit broadcastOutputText(output_text);
                        return false;
                    }
                    Point2f p(x,y);
                    region.push_back(p);
                    count++;
                }
                node = node.nextSibling();

                if (count-result==1000)
				{
                    result=count;
                    float division = (float)size / (float)count;
                    int temp =  85/division;
					emit progressValueChanged(temp);
                }
            }
            qDebug() << region.size();
            return true;
        }
    }

}

/*
 * Save image
 */
void ActualDetector::saveImg(string path, Mat & image)
{
    stringstream ss;
    ss << path << imageCount << ext;
    imwrite(ss.str(), image );
}

/*
 * Stop the Detection process
 */
void ActualDetector::stopThread()
{
    region.clear();
    isActive = false;
    theRecorder.stopRecording(true);
    if (pThread) 
	{
        pThread->join(); 
		pThread.reset();
        this_thread::sleep_for(chrono::seconds(1));
        threadNightChecker->join(); threadNightChecker.reset();
    }
}

/*
 * Called from MainWindow to start the Detection process
 */
bool ActualDetector::start()
{
    if (!pThread)
	{
		emit progressValueChanged(1); 
        if(initialize())
		{
            isActive=true;
            threadNightChecker.reset(new std::thread(&ActualDetector::checkIfNight, this));
			emit progressValueChanged(90);
            this_thread::sleep_for(chrono::seconds(1));
			emit progressValueChanged(100);

            if (!pThread)
			{
				pThread.reset(new std::thread(&ActualDetector::detectingThread, this));
			}
            return true;
        }
        else return false;
    }
    return true;
}

Rect ActualDetector::enlargeROI(Mat &frm, Rect &boundingBox, int padding) 
{
    Rect returnRect = Rect(boundingBox.x - padding, boundingBox.y - padding, boundingBox.width + (padding * 2), boundingBox.height + (padding * 2));
    if (returnRect.x < 0)returnRect.x = 0;
    if (returnRect.y < 0)returnRect.y = 0;
    if (returnRect.x+returnRect.width >= frm.cols)returnRect.width = frm.cols-returnRect.x;
    if (returnRect.y+returnRect.height >= frm.rows)returnRect.height = frm.rows-returnRect.y;
    return returnRect;
}


void ActualDetector::setNoiseLevel(int level)
{
    kernel_ero=getStructuringElement(MORPH_RECT, Size(level,level));
}

void ActualDetector::setThresholdLevel(int level)
{
    thresholdLevel=level;
}

void ActualDetector::setFilename(string msg)
{
    filename=msg;
}

void ActualDetector::startRecording()
{
    theRecorder.setup(result);
}

Recorder* ActualDetector::getRecorder()
{
    return &theRecorder;
}

