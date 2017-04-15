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

ActualDetector::ActualDetector(Camera* camera, Config* config, DataManager* dataManager, QObject *parent) :
    QObject(parent), m_camPtr(camera)
{
    m_config = config;
    m_dataManager = dataManager;
    const QString DETECTION_AREA_FILE = m_config->detectionAreaFile();
    const QString IMAGEPATH = m_config->resultImageDir() + "/";
    m_willSaveImages = m_config->saveResultImages();
    m_cameraWidth = m_config->cameraWidth();
    m_cameraHeight = m_config->cameraHeight();
    m_willRecordWithRect = m_config->resultVideoWithObjectRectangles();
    m_isMainThreadRunning = false;
    m_showCameraVideo = false;
    m_startedRecording = false;

    m_detectionAreaFile = DETECTION_AREA_FILE.toStdString();
    m_resultImageDirNameBase = IMAGEPATH.toStdString();

    setNoiseLevel(m_config->noiseFilterPixelSize());
    setThresholdLevel(m_config->motionThreshold());

    m_recorder = new Recorder(m_camPtr, m_config, m_dataManager);

    state = new DetectorState(this, m_recorder);
    state->MIN_POS_REQUIRED = m_config->minPositiveDetections();
    connect(state, SIGNAL(sendOutputText(QString)), this, SIGNAL(broadcastOutputText(QString)));

    loadDetectionArea(m_config->cameraIndex());

    qDebug() << "ActualDetector constructed";
}

ActualDetector::~ActualDetector()
{
    m_recorder->deleteLater();
    state->deleteLater();
}

bool ActualDetector::initialize()
{
    chrono::high_resolution_clock::time_point startTime, endTime;
    startTime = chrono::high_resolution_clock::now();
    if (!parseDetectionAreaFile(m_detectionAreaFile, m_region)){
        return false;
    }
    endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> msec = endTime - startTime;
    qDebug() << "Parsing detection area file took" << msec.count() << "ms";
    state->resetState();

    m_prevFrame = m_camPtr->getWebcamFrame();
    m_resultFrame = m_prevFrame;
    m_currentFrame = m_camPtr->getWebcamFrame();
    m_nextFrame = m_camPtr->getWebcamFrame();
    cvtColor(m_currentFrame, m_currentFrame, CV_RGB2GRAY);
    cvtColor(m_prevFrame, m_prevFrame, CV_RGB2GRAY);
    cvtColor(m_nextFrame, m_nextFrame, CV_RGB2GRAY);

    m_minAmountOfMotion = 2;
    m_maxDeviation = 20;
    m_imageCount = 0;
    m_savedImageExtension =".jpg";


    m_willParseRectangle = false;
    if(m_willSaveImages)
    {
        string dateTime=QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss").toStdString();
        QDir dir = QDir::root();
        dir.mkpath(QString::fromStdString(m_resultImageDirNameBase + dateTime));
        m_resultImageDirName = (QString::fromStdString(m_resultImageDirNameBase + dateTime +"/"+"image")).toLatin1().toStdString();
        m_pathNameThresh = m_resultImageDirName + "Thresh";
    }

    m_isInNightMode = false;
    m_isCascadeFound = true;
    if (!m_birdsCascade.load(m_config->birdClassifierTrainingFile().toStdString()))
    {
        auto output_text = tr("WARNING: could not load bird detection data (cascade classifier file)");
        qWarning() << output_text;
        emit broadcastOutputText(output_text);
        m_isInNightMode = true;
        m_isCascadeFound = false;
    }

    qDebug() << "Initialized ActualDetector";
    this_thread::sleep_for(std::chrono::seconds(1));
    m_startedRecording = false;


    m_rect = Rect(Point(0,0),Point(m_cameraWidth,m_cameraHeight));
    m_treshImg = m_resultFrame.clone();
    m_treshImg.setTo(Scalar(0,0,0));

    return true;
}

/*
 * The detection thread
 */
void ActualDetector::detectingThread()
{    
    Mat tempImage;
    int counterNoMotion = 0;
    int counterBlackDetecor = 0;
    int counterLight = 0;
    bool isPositiveRectangle;
    int threadYieldPauseUsec = 1000;
    int numberOfChanges = 0;
    int frameCount = 0;
    int framesInFpsMeasurement = OUTPUT_FPS * 10;
    bool fpsMeasurementDone = false;
    QTime fpsMeasurementTimer;

    CDetector* detector=new CDetector(m_currentFrame);
    vector<Point2d> centers;
    Scalar Colors[]={Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255),Scalar(255,255,0),Scalar(0,255,255),Scalar(255,0,255),Scalar(255,127,255),Scalar(127,0,255),Scalar(127,0,127)};
    pair < vector<Point2d>,vector<Rect> > centerAndRectPair;

    qDebug() << "ActualDetector::detectingThread() started";

    fpsMeasurementTimer.start();

    while (m_isMainThreadRunning)
    {
        m_prevFrame = m_currentFrame;
        m_currentFrame = m_nextFrame;
        tempImage = m_camPtr->getWebcamFrame();
        frameCount++;
        if (!fpsMeasurementDone && (frameCount >= framesInFpsMeasurement)) {
            qDebug() << "ActualDetector reading" << ((float)frameCount /
                         (float)fpsMeasurementTimer.elapsed()) * (float)1000
                     << "FPS on average";
            fpsMeasurementDone = true;
        }

        m_nextFrame = tempImage.clone();
        m_resultFrame = m_nextFrame;
        cvtColor(m_nextFrame, m_nextFrame, CV_RGB2GRAY);

        absdiff(m_prevFrame, m_nextFrame, m_d1);
        absdiff(m_currentFrame, m_nextFrame, m_d2);
        bitwise_and(m_d1, m_d2, m_motion);
        threshold(m_motion, m_motion, m_thresholdLevel, 255, CV_THRESH_BINARY);

        erode(m_motion, m_motion, m_noiseLevel);

        numberOfChanges = detectMotion(m_motion, m_resultFrame, m_resultFrameCropped, m_region, m_maxDeviation);

        if(numberOfChanges>=m_minAmountOfMotion)
        {
            centerAndRectPair = detector->Detect(m_treshImg,m_rect);
            centers = centerAndRectPair.first;
            m_detectorRectVec = centerAndRectPair.second;
            counterNoMotion=0;
            if(centers.size()>0)
            {
                state->tracker.Update(centers,m_detectorRectVec,CTracker::RectsDist);
            }
            //loop through detected objects
            if (m_detectorRectVec.size()<  MAX_OBJECTS_IN_FRAME)
            {
                isPositiveRectangle=false;
                for ( unsigned int i=0;i<m_detectorRectVec.size();i++)
                {
                    Rect croppedRectangle = m_detectorRectVec[i];
                    Mat temp = m_resultFrame(croppedRectangle);
                    Mat croppedImage = temp.clone();
                    //+++check if there was light in object
                    if(lightDetection(croppedRectangle,croppedImage))
                    {
                        //object was bright
                        if (!m_isInNightMode && checkIfBird())
                        {
                            state->tracker.tracks[i]->birdCounter++;
                            state->tracker.tracks[i]->negCounter++;
                        }
                        else
                        {//+++ not in night mode or was not a bird*/
                            counterLight++;
                            if(counterLight>2)counterBlackDetecor=0;
                            if(counterBlackDetecor<5)
                            {
                                emit checkPlane();
                                if(!m_startedRecording)
                                {
                                    Mat tempImg = m_resultFrame.clone();
                                    rectangle(tempImg,croppedRectangle,Scalar(255,0,0),1);
                                    m_recorder->startRecording(tempImg);
                                    if(m_willRecordWithRect) m_willParseRectangle=true;
                                    m_startedRecording=true;
                                    auto output_text = tr("Positive detection - starting video recording");
                                    emit broadcastOutputText(output_text);
                                }

                                if(m_willParseRectangle)
                                {
                                    isPositiveRectangle=true;
                                }
                                state->negAndNoMotionCounter=0;
                                state->posCounter++;
                                state->tracker.tracks[i]->posCounter++;
                                emit positiveMessage();

                                if(m_willSaveImages)
                                {
                                    saveImg(m_resultImageDirName, croppedImage);
                                    m_imageCount++;
                                    //saveImg(pathnameThresh, treshImg);
                                }
                            }
                        }
                    }

                    else { //+++motion has black pixel
                        counterBlackDetecor++;
                        counterLight=0;
                        state->tracker.tracks[i]->negCounter++;

                        if (m_startedRecording)
                        {
                            state->negAndNoMotionCounter++;
                        }
                        emit negativeMessage();
                    }
                }
                if(m_willParseRectangle)
                {
                    m_recorder->setRectangle(m_rect,isPositiveRectangle);
                }

            }
        }
        else
        { //+++no motion detected
            counterLight=0;
            counterBlackDetecor=0;
            counterNoMotion++;
            if (m_startedRecording)
            {
                state->negAndNoMotionCounter++;
            }
            state->tracker.updateEmpty();
            centers.clear();
            m_detectorRectVec.clear();
            if ((m_startedRecording && counterNoMotion > 150) || (state->negAndNoMotionCounter > 700))
            {
                state->finishRecording();
                state->resetState();
                m_willParseRectangle=false;
                m_startedRecording=false;
            }
        }

        // check if there was a plane
        if (state->numberOfPlanes && state->numberOfPlanes >= centers.size()){
            state->wasPlane = true;
        }

        if (m_showCameraVideo && centers.size() < MAX_OBJECTS_IN_FRAME )
        {
            for(unsigned int i=0; i<centers.size(); i++)
            {
                //rectangle(result,detectorRectVec[i],color,1);
                circle(m_resultFrame,centers[i],3,Scalar(0,255,0),1,CV_AA);
                // stringstream ss;
                // char str[256] = "";
                // snprintf(str, sizeof(str), "%zu", tracker.tracks[i]->track_id);
                // ss << str << " P: " << tracker.tracks[i]->posCounter << " N: " << tracker.tracks[i]->negCounter;
                // putText(result,ss.str(),centers[i],CV_FONT_HERSHEY_PLAIN,2, CV_RGB(250,0,0));
            }
            if(centers.size()>0)
            {
                for(unsigned int i=0;i<state->tracker.tracks.size();i++)
                {
                    if(!state->tracker.tracks[i]->trace.empty())
                    {
                        for(unsigned int j=0;j<state->tracker.tracks[i]->trace.size()-1;j++)
                        {
                            line(m_resultFrame,state->tracker.tracks[i]->trace[j],state->tracker.tracks[i]->trace[j+1],Colors[state->tracker.tracks[i]->track_id%9],2,CV_AA);
                        }
                    }
                }
            }

            cv::cvtColor(m_resultFrame, m_resultFrame, CV_BGR2RGB);
            m_cameraViewImage = QImage((uchar*)m_resultFrame.data, m_resultFrame.cols, m_resultFrame.rows, m_resultFrame.step, QImage::Format_RGB888);
            emit updatePixmap(m_cameraViewImage.copy());
        }
        // give chance to other threads to run
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::microseconds(threadYieldPauseUsec));
    }
    qDebug() << "ActualDetector::detectingThread() finished";
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
            m_rect = Rect(x,y);
            Mat croppedThresh = motion(m_rect);
            croppedThresh.copyTo(m_treshImg);
            Mat cropped = result(m_rect);
            cropped.copyTo(result_cropped);

        }
        else
        {
            m_rect = Rect(Point(0,0),Point(m_cameraWidth,m_cameraHeight));
            m_treshImg = result.clone();
            m_treshImg.setTo(Scalar(0,0,0));
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
    croppedImageThreshTemp = m_motion(rectangle);
    croppedImageThreshTemp.copyTo(croppedImageThresh);
    cvtColor(croppedImage, m_croppedImageGray , CV_RGB2GRAY);

    int light=0;
    int totalLight=0;
    bool wasDark=false;

    for(int y = 0; y < croppedImage.rows; y++)
    {
        for(int x = 0; x < croppedImage.cols; x++)
        {
            light+=static_cast<int>(m_croppedImageGray.at<uchar>(y,x));
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
        if(static_cast<int>(m_croppedImageGray.at<uchar>(y,x)) > minAndMaxLight.first)
        {
            lightCounter++;
        }
        if(static_cast<int>(m_croppedImageGray.at<uchar>(y,x)) < minAndMaxLight.second)
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
 * Thread that check if it is night every 300 seconds (5mins)
 * If total brightness is less than 100 it is night. In that case we temporary stop the detection process
 * to exclude any area from the detection area which is bright (i.e. the moon and stars) in order to
 * ignore any image noise around that area
 */
void ActualDetector::checkIfNight()
{
    bool isRunning = true;
    vector<Point> regionBackup=m_region;
    vector<Point> regionNew;
    bool changedRegion=false;
    int timerSeconds=300;

    while(isRunning)
    {
        //get average brightness of region
        int total=0;
        long light=0;
        Mat temp;
        temp = m_camPtr->getWebcamFrame();
        Mat frame = temp.clone();
        cvtColor(frame, frame , CV_RGB2GRAY);

        for(unsigned int i = 0; i<m_region.size(); i++)
        {
            int x = m_region[i].x;
            int y = m_region[i].y;
            light+=static_cast<int>(frame.at<uchar>(y,x));
        }

        total = light/m_region.size();

        if (total<100)
        {
            stopOnlyDetecting();
            m_region=regionBackup;
            m_isInNightMode=true;


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
                regionNew=m_region;
                vector<Point>::iterator it = regionNew.begin();
                for ( ; it != regionNew.end(); )
                {
                    if (static_cast<int>(imageBinary.at<uchar>(*it)) != 0)
                    {
                        it = regionNew.erase(it);
                    }
                    else ++it;
                }

                auto output_text = tr("%1 area(s) being ignored in order to filter the moon and stars").arg(QString::number(constants.size()));
                emit broadcastOutputText(output_text);
                changedRegion=true;

            }
            if (!m_mainThread)
            {
                if(changedRegion)
                {
                    m_region=regionNew;
                    changedRegion=false;
                }
                regionNew.clear();
                m_isMainThreadRunning=true;
                m_mainThread.reset(new std::thread(&ActualDetector::detectingThread, this));
            }

        }
        else if (m_isCascadeFound)
        {
            m_isInNightMode=false;
        }


        //timer
        int counter=0;
        bool isSleeping=true;
        while(isSleeping)
        {
            if(m_isMainThreadRunning && counter<timerSeconds)
            {
                this_thread::sleep_for(chrono::seconds(1));
                counter++;
            }
            else if(m_isMainThreadRunning && counter==timerSeconds)
            {
                isSleeping=false;
                qDebug() << "re run loop and timer";
            }
            else if(!m_isMainThreadRunning)
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
    if(m_startedRecording)
    {
        qDebug() << "wait till stopped recording";
        this_thread::sleep_for(chrono::seconds(1));
        stopOnlyDetecting();
    }
    else
    {
        if (m_mainThread)
        {
            m_isMainThreadRunning=false;
            m_mainThread->join();
            m_mainThread.reset();
        }
    }
}

bool ActualDetector::checkIfBird()
{
    std::vector<Rect> birds;
    m_birdsCascade.detectMultiScale(m_croppedImageGray, birds, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(CLASSIFIER_DIMENSION_SIZE, CLASSIFIER_DIMENSION_SIZE) );

    return (!birds.empty());
}

/*
 * Get vector with the Rect of all constant bright objects
 */
std::vector<Rect> ActualDetector::getConstantRecs(int totalLight)
{
    vector<Rect> rectVec;
    Mat image = m_camPtr->getWebcamFrame();
    Mat imageGray;
    cvtColor(image, imageGray , CV_RGB2GRAY);

    int y, x, size;
    size=m_region.size();
    int minLight = checkBrightness(totalLight).first;

    //find bright pixels in webcam frame and paint pixels in binary image
    Mat imageBinary(image.rows,image.cols,CV_THRESH_BINARY, Scalar(0,0,0));
    for(int i = 0; i < size; i++)
    {
        x = m_region[i].x;
        y = m_region[i].y;
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

    if(!fileXML.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open the detection area file" << QString(file_region.data());
        emit errorReadingDetectionAreaFile();
        return false;
    }
    else
    {
        if(!doc.setContent(&fileXML))
        {
            qDebug() << "Error reading the detection area file" << QString(file_region.data());
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
                    if(y>m_cameraHeight || x>m_cameraWidth)
                    {
                        qDebug() << y;
                        qDebug() << x;
                        /// @todo auto-scale area of detection
                        auto output_text = tr("ERROR: Selected area of detection does not match camera resolution. Re-select area of detection.");
                        emit broadcastOutputText(output_text);
                        return false;
                    }
                    Point2f p(x,y);
                    region.push_back(p);
                    count++;
                }
                node = node.nextSibling();

                if (count%1000 == 0)
                {
                    float division = (float)size / (float)count;
                    int percentage =  85/division;
                    emit progressValueChanged(percentage);
                }
            }
            return true;
        }
    }

}

bool ActualDetector::loadDetectionArea(int cameraId) {
    QFileInfo origFile(QFile(m_detectionAreaFile.c_str()));
    QFile areaFile(origFile.absolutePath() + "/detectionArea-poly.xml");
    QDomDocument doc;
    QDomElement root;
    QList<QPolygon> polygons;
    int size = 0;
    int x = 0;
    int y = 0;
    chrono::high_resolution_clock::time_point startTime, endTime;

    startTime = chrono::high_resolution_clock::now();
    if(!areaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open the detection area file" << areaFile.fileName();
        emit errorReadingDetectionAreaFile();
        return false;
    }

    if(!doc.setContent(&areaFile)) {
        qDebug() << "Error reading the detection area file" << areaFile.fileName();
        areaFile.close();
        emit errorReadingDetectionAreaFile();
        return false;
    }

    areaFile.close();
    root = doc.documentElement();
    if (root.nodeName() != "detectionarealist") {
        QString errorMsg = tr("Expected <detectionarealist> tag in detection area file but not found");
        emit broadcastOutputText(errorMsg);
        return false;
    }
    QDomNodeList areaList = root.childNodes();
    QRect cameraRect(0, 0, m_config->cameraWidth(), m_config->cameraHeight());

    if (areaList.count() > 1) {
        QString areaCountWarning = tr("More than one detection areas defined, combining all together");
        emit broadcastOutputText(areaCountWarning);
    }

    m_region.clear();

    for (int i = 0; i < areaList.count(); i++) {
        QDomNode area = areaList.at(i);
        QDomNodeList areaNodes = area.childNodes();
        if (area.nodeName() != "detectionarea") {
            QString errorMsg = tr("Expected <detectionarea> tag in detection area file but not found.");
            emit broadcastOutputText(errorMsg);
            return false;
        }
        QDomNodeList cameraList = area.toElement().elementsByTagName("camera");
        if (cameraList.count() != 1) {
            QString errorMsg = tr("Expected single <camera> tag in detection area. Assuming camera index 0.");
            emit broadcastOutputText(errorMsg);
        }

        for (int a = 0; a < areaNodes.count(); a++) {
            QDomNode areaSubNode = areaNodes.at(a);
            if (areaSubNode.nodeName() == "polygon") {
                QDomNodeList pointList = areaSubNode.childNodes();
                QPolygon polygon;
                for (int p = 0; p < pointList.count(); p++) {
                    QDomElement pointElement = pointList.at(p).toElement();
                    if (pointElement.nodeName() == "point") {
                        x = pointElement.attribute("x").toInt();
                        y = pointElement.attribute("y").toInt();
                        polygon << QPoint(x, y);
                    }
                }
                qDebug() << polygon;
                QRect boundingRect = polygon.boundingRect();
                if (!cameraRect.contains(boundingRect)) {
                    QString errorMsg = tr("ERROR: Selected area of detection does not match camera resolution. Re-select area of detection.");
                    emit broadcastOutputText(errorMsg);
                    return false;
                }
                for (int bx = boundingRect.x(); bx < boundingRect.width(); bx++) {
                    for (int by = boundingRect.y(); by < boundingRect.height(); by++) {
                        if (polygon.containsPoint(QPoint(bx, by), Qt::OddEvenFill)) {
                            m_region.push_back(cv::Point2f(bx, by));
                            size++;
                        }
                    }
                }
            }
        }
    }
    endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> timeMsec = endTime - startTime;
    qDebug() << "Got" << size << "points in detection area";
    qDebug() << "Parsing detection area polygon file took" << QString::number(timeMsec.count()) << "ms";
    return true;
}

/*
 * Save image
 */
void ActualDetector::saveImg(string path, Mat & image)
{
    stringstream ss;
    ss << path << m_imageCount << m_savedImageExtension;
    imwrite(ss.str(), image );
}

/*
 * Stop the Detection process
 */
void ActualDetector::stopThread()
{
    m_region.clear();
    m_isMainThreadRunning = false;
    m_recorder->stopRecording(true);
    if (m_mainThread)
    {
        m_mainThread->join();
        m_mainThread.reset();
        this_thread::sleep_for(chrono::seconds(1));
        m_nightCheckerThread->join(); m_nightCheckerThread.reset();
    }
}

bool ActualDetector::start()
{
    if (!m_mainThread)
    {
        emit progressValueChanged(1);
        if(initialize())
        {
            m_isMainThreadRunning=true;
            m_nightCheckerThread.reset(new std::thread(&ActualDetector::checkIfNight, this));
            emit progressValueChanged(90);
            this_thread::sleep_for(chrono::seconds(1));
            emit progressValueChanged(100);

            if (!m_mainThread)
            {
                m_mainThread.reset(new std::thread(&ActualDetector::detectingThread, this));
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

void ActualDetector::setAmountOfPlanes(int amount)
{
    state->numberOfPlanes = amount;
}


void ActualDetector::setNoiseLevel(int level)
{
    m_noiseLevel=getStructuringElement(MORPH_RECT, Size(level,level));
}

void ActualDetector::setThresholdLevel(int level)
{
    m_thresholdLevel=level;
}

void ActualDetector::startRecording()
{
    Mat firstFrame = m_resultFrame.clone();
    m_recorder->startRecording(firstFrame);
}


Recorder* ActualDetector::getRecorder()
{
    return m_recorder;
}

void ActualDetector::setShowCameraVideo(bool show)
{
    m_showCameraVideo = show;
}

