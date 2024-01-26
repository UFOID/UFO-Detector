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

#include <QElapsedTimer>
#include <QRect>
#include <memory>
#include "actualdetector.h"
#include "Detector.h"

ActualDetector::ActualDetector(Camera *camera, Config *config,
                               DataManager *dataManager, QObject *parent) :
        QObject(parent), m_camPtr(camera) {
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

    m_recorder = std::make_unique<Recorder>(m_camPtr, m_config);
    state = std::make_unique<DetectorState>(this, m_recorder.get());

    state->MIN_POS_REQUIRED = m_config->minPositiveDetections();
    connect(m_recorder.get(), &Recorder::recordingFinished,
            this, &ActualDetector::recordingFinished);
    connect(state.get(), SIGNAL(sendOutputText(QString)), this, SIGNAL(broadcastOutputText(QString)));
}

ActualDetector::~ActualDetector() {
    m_recorder->deleteLater();
    state->deleteLater();
}

bool ActualDetector::initialize() {
    if (!initDetectionArea()) {
        return false;
    }
    state->resetState();

    m_prevFrame = m_camPtr->getFrameDetector().frame;
    m_resultFrame = m_prevFrame;
    m_currentFrame = m_camPtr->getFrameDetector().frame;
    m_nextFrame = m_camPtr->getFrameDetector().frame;
    cvtColor(m_currentFrame, m_currentFrame, COLOR_RGB2GRAY);
    cvtColor(m_prevFrame, m_prevFrame, COLOR_RGB2GRAY);
    cvtColor(m_nextFrame, m_nextFrame, COLOR_RGB2GRAY);

    m_minAmountOfMotion = 2;
    m_maxDeviation = 20;
    m_imageCount = 0;
    m_savedImageExtension = ".jpg";


    m_willParseRectangle = false;
    if (m_willSaveImages) {
        string dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss").toStdString();
        QDir dir = QDir::root();
        dir.mkpath(QString::fromStdString(m_resultImageDirNameBase + dateTime));
        m_resultImageDirName = m_resultImageDirNameBase + dateTime + "/image";
        m_pathNameThresh = m_resultImageDirName + "Thresh";
    }

    m_isInNightMode = false;
    m_isCascadeFound = true;
    if (!m_birdsCascade.load(m_config->birdClassifierTrainingFile().toStdString())) {
        auto output_text = tr("WARNING: could not load bird detection data (cascade classifier file)");
        qWarning() << output_text;
        m_isInNightMode = true;
        m_isCascadeFound = false;
    }

    qInfo() << "Initialized ActualDetector";
    this_thread::sleep_for(std::chrono::seconds(1));
    m_startedRecording = false;


    m_rect = Rect(Point(0, 0), Point(m_cameraWidth, m_cameraHeight));
    m_treshImg = Mat::zeros(m_resultFrame.size(), m_resultFrame.type());

    return true;
}

/*
 * The detection thread
 */
void ActualDetector::detectingThread() {
    int counterNoMotion = 0;
    int counterBlackDetecor = 0;
    int counterLight = 0;
    bool isPositiveRectangle;
    int threadYieldPauseUsec = 1000;
    int numberOfChanges = 0;
    int frameCount = 0;
    int framesInFpsMeasurement = OUTPUT_FPS * 10;
    bool fpsMeasurementDone = false;
    QElapsedTimer fpsMeasurementTimer;

    auto detector = std::make_unique<CDetector>(m_currentFrame);
    vector<Point2d> centers;
    Scalar Colors[] = {Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(0, 0, 255), Scalar(255, 255, 0),
                       Scalar(0, 255, 255), Scalar(255, 0, 255), Scalar(255, 127, 255), Scalar(127, 0, 255),
                       Scalar(127, 0, 127)};
    pair<vector<Point2d>, vector<Rect> > centerAndRectPair;

    qInfo() << ("ActualDetector::detectingThread() started");

    fpsMeasurementTimer.start();

    using namespace std::chrono;
    auto lastTime = steady_clock::now();

    while (m_isMainThreadRunning) {
        m_prevFrame = m_currentFrame;
        m_currentFrame = m_nextFrame;
        auto timedFrame = m_camPtr->getFrameDetector();
        if (timedFrame.frame.empty()) {
            qInfo() << ("ERROR: detector Failed to read frame from camera");
            continue;
        }
        m_nextFrame = timedFrame.frame;
        auto timestamp = timedFrame.timestamp;
        m_resultFrame = m_nextFrame;
        frameCount++;
        if (!fpsMeasurementDone && (frameCount >= framesInFpsMeasurement)) {
            float fps = ((float) frameCount / (float) fpsMeasurementTimer.elapsed()) * (float) 1000;
            qInfo() << ("ActualDetector reading " + QString::number(fps) + " FPS on average");
            fpsMeasurementDone = true;
        }

//        frameCount++;
//        auto currentTime = steady_clock::now();
//        duration<double> elapsed = currentTime - lastTime;
//
//        if (elapsed.count() >= 1.0) {  // Every second
//            double fps = frameCount / elapsed.count();
//            std::cout << "Thread  FPS: " << fps << std::endl;
//            frameCount = 0;
//            lastTime = currentTime;
//        }

        cvtColor(m_nextFrame, m_nextFrame, COLOR_RGB2GRAY);

        absdiff(m_prevFrame, m_nextFrame, m_d1);
        absdiff(m_currentFrame, m_nextFrame, m_d2);
        bitwise_and(m_d1, m_d2, m_motion);
        threshold(m_motion, m_motion, m_thresholdLevel, 255, THRESH_BINARY);

        erode(m_motion, m_motion, m_noiseLevel);

        numberOfChanges = detectMotion(m_motion, m_resultFrame, m_region, m_maxDeviation);

        if (numberOfChanges >= m_minAmountOfMotion) {
            centerAndRectPair = detector->Detect(m_treshImg, m_rect);
            centers = centerAndRectPair.first;
            m_detectorRectVec = centerAndRectPair.second;
            counterNoMotion = 0;
            if (centers.size() > 0) {
                state->tracker.Update(centers, m_detectorRectVec, CTracker::RectsDist);
            }
            //loop through detected objects
            if (m_detectorRectVec.size() < MAX_OBJECTS_IN_FRAME) {
                isPositiveRectangle = false;
                for (unsigned int i = 0; i < m_detectorRectVec.size(); i++) {
                    Rect croppedRectangle = m_detectorRectVec[i];
                    Mat croppedImage = m_resultFrame(croppedRectangle);
                    //+++check if there was light in object
                    if (lightDetection(croppedRectangle, croppedImage)) {
                        //object was bright
                        if (!m_isInNightMode && checkIfBird()) {
                            state->tracker.tracks[i]->birdCounter++;
                        } else {//+++ not in night mode or was not a bird*/
                            counterLight++;
                            if (counterLight > 2)counterBlackDetecor = 0;
                            if (counterBlackDetecor < 5) {
                                emit checkPlane();
                                if (!m_startedRecording) {
                                    Mat tempImg = m_resultFrame.clone();
                                    rectangle(tempImg, croppedRectangle, Scalar(255, 0, 0), 1);
                                    if (m_recorder->startRecording(TimedMat(tempImg, timestamp))){
                                        if (m_willRecordWithRect) m_willParseRectangle = true;
                                        m_startedRecording = true;
                                        auto output_text = tr("Positive detection - starting video recording");
                                        emit broadcastOutputText(output_text);
                                    }
                                }

                                if (m_willParseRectangle) {
                                    isPositiveRectangle = true;
                                }
                                state->negAndNoMotionCounter = 0;
                                state->posCounter++;
                                state->tracker.tracks[i]->posCounter++;
                                emit positiveMessage();

                                if (m_willSaveImages) {
                                    saveImg(m_resultImageDirName, croppedImage);
                                    m_imageCount++;
                                    //saveImg(pathnameThresh, treshImg);
                                }
                            }
                        }
                    } else { //+++motion has black pixel
                        counterBlackDetecor++;
                        counterLight = 0;
                        state->tracker.tracks[i]->negCounter++;

                        if (m_startedRecording) {
                            state->negAndNoMotionCounter++;
                        }
                        emit negativeMessage();
                    }
                }
                if (m_willParseRectangle) {
                    m_recorder->setRectangle(m_rect, isPositiveRectangle);
                }

            }
        } else { //+++no motion detected
            counterLight = 0;
            counterBlackDetecor = 0;
            counterNoMotion++;
            if (m_startedRecording) {
                state->negAndNoMotionCounter++;
            }
            state->tracker.updateEmpty();
            centers.clear();
            m_detectorRectVec.clear();
            if ((m_startedRecording && counterNoMotion > 150) || (state->negAndNoMotionCounter > 700)) {
                state->finishRecording();
                state->resetState();
                m_willParseRectangle = false;
                m_startedRecording = false;
            }
        }

        // check if there was a plane
        if (state->numberOfPlanes && state->numberOfPlanes >= centers.size()) {
            state->wasPlane = true;
        }

        if (m_showCameraVideo && centers.size() < MAX_OBJECTS_IN_FRAME) {
            for (auto &center: centers) {
                //rectangle(result,detectorRectVec[i],color,1);
                circle(m_resultFrame, center, 3, Scalar(0, 255, 0), 1, LINE_AA);
                // stringstream ss;
                // char str[256] = "";
                // snprintf(str, sizeof(str), "%zu", tracker.tracks[i]->track_id);
                // ss << str << " P: " << tracker.tracks[i]->posCounter << " N: " << tracker.tracks[i]->negCounter;
                // putText(result,ss.str(),centers[i],CV_FONT_HERSHEY_PLAIN,2, CV_RGB(250,0,0));
            }
            if (!centers.empty()) {
                for (unsigned int i = 0; i < state->tracker.tracks.size(); i++) {
                    if (!state->tracker.tracks[i]->trace.empty()) {
                        for (unsigned int j = 0; j < state->tracker.tracks[i]->trace.size() - 1; j++) {
                            line(m_resultFrame, state->tracker.tracks[i]->trace[j],
                                 state->tracker.tracks[i]->trace[j + 1], Colors[state->tracker.tracks[i]->track_id % 9],
                                 2, LINE_AA);
                        }
                    }
                }
            }

            cv::cvtColor(m_resultFrame, m_resultFrame, COLOR_BGR2RGB);
//            m_cameraViewImage = QImage((uchar*)m_resultFrame.data, m_resultFrame.cols, m_resultFrame.rows, m_resultFrame.step, QImage::Format_RGB888);
            emit updateFrame(m_resultFrame);
        }

        m_prevFrame.release();

        // give chance to other threads to run
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::microseconds(threadYieldPauseUsec));
    }
    qInfo() << ("ActualDetector::detectingThread() finished");
}

/*
 * Check if there was motion between frames. Return the AmountOfMotion detected
 */
inline int ActualDetector::detectMotion(const Mat &motion, Mat &result, vector<Point> &region, int max_deviation) {
    // calculate the standard deviation
    Scalar mean, stddev;
    meanStdDev(motion, mean, stddev);
    // if not to much changes then the motion is real
    if (stddev[0] < max_deviation) {
        int number_of_changes = 0;
        int min_x = motion.cols, max_x = 0;
        int min_y = motion.rows, max_y = 0;
        // loop over image and detect changes
        int x, y, size = region.size();

        for (int i = 0; i < size; i++) { // loop over region
            x = region[i].x;
            y = region[i].y;
            if (static_cast<int>(motion.at<uchar>(y, x)) == 255) {
                number_of_changes++;
                if (min_x > x) min_x = x;
                if (max_x < x) max_x = x;
                if (min_y > y) min_y = y;
                if (max_y < y) max_y = y;
            }
        }
        if (number_of_changes) {
            //check if not out of bounds
            if (min_x - 10 > 0) min_x -= 10;
            if (min_y - 10 > 0) min_y -= 10;
            if (max_x + 10 < result.cols - 1) max_x += 10;
            if (max_y + 10 < result.rows - 1) max_y += 10;

            Point x(min_x, min_y);
            Point y(max_x, max_y);
            m_rect = Rect(x, y);
            Mat croppedThresh = motion(m_rect);
            croppedThresh.copyTo(m_treshImg);
        } else {
            m_rect = Rect(Point(0, 0), Point(m_cameraWidth, m_cameraHeight));
            m_treshImg = Mat::zeros(result.size(), result.type()); // Create a new matrix of zeros
        }
        return number_of_changes;
    }
    return 0;
}

bool ActualDetector::initDetectionArea() {

    bool readOk = m_dataManager->readDetectionAreaFile(true);
    if (!readOk) {
        qWarning() << "ERROR: Could not read detection area file";
        return false;
    }

    cv::Rect cameraRect(0, 0, m_config->cameraWidth(), m_config->cameraHeight());

    QList<std::vector<cv::Point>> polygonList = m_dataManager->detectionArea();
    for (const std::vector<cv::Point> &polygon: polygonList) {
        cv::Rect boundingRect = cv::boundingRect(polygon);

        if (!cameraRect.contains(boundingRect.tl()) || !cameraRect.contains(boundingRect.br())) {
            QString errorMsg = tr(
                    "ERROR: Selected area of detection does not match camera resolution. Re-select area of detection.");
            qWarning() << errorMsg;
            return false;
        }

        for (int bx = boundingRect.x; bx < boundingRect.x + boundingRect.width; bx++) {
            for (int by = boundingRect.y; by < boundingRect.y + boundingRect.height; by++) {
                if (cv::pointPolygonTest(polygon, cv::Point2f(bx, by), false) > 0) {
                    m_region.push_back(cv::Point2f(bx, by));
                }
            }
        }
    }
    qInfo() << m_region.size() << " points in detection area";
    return true;
}

/*
 * Check if an object is bright. I.e. object has more bright pixels than dark pixels
 */
bool ActualDetector::lightDetection(Rect &rectangle, Mat &croppedImage) {
    bool objectHasLight = false;

    vector<cv::Point> detectionRegion;

    int lightCounter = 0;
    int blackCounter = 0;
    Mat croppedImageThresh = m_motion(rectangle);
    cvtColor(croppedImage, m_croppedImageGray, COLOR_RGB2GRAY);

    int light = 0;
    int totalLight = 0;
    bool wasDark = false;

    for (int y = 0; y < croppedImage.rows; y++) {
        for (int x = 0; x < croppedImage.cols; x++) {
            light += static_cast<int>(m_croppedImageGray.at<uchar>(y, x));
            if (static_cast<int>(croppedImageThresh.at<uchar>(y, x)) == 255) {
                Point2f p(x, y);
                detectionRegion.push_back(p);
            }
        }
    }

    totalLight = light / (croppedImage.cols * croppedImage.rows);
    pair<int, int> minAndMaxLight = checkBrightness(totalLight);

    int x, y, size = detectionRegion.size();
    for (int j = 0; j < size; j++) {
        x = detectionRegion[j].x;
        y = detectionRegion[j].y;
        if (static_cast<int>(m_croppedImageGray.at<uchar>(y, x)) > minAndMaxLight.first) {
            lightCounter++;
        }
        if (static_cast<int>(m_croppedImageGray.at<uchar>(y, x)) < minAndMaxLight.second) {
            blackCounter++;
        }
    }

    if (totalLight < 26) {  //Activate night mode for object
        wasDark = true;
    }

    if (wasDark) {
        if (lightCounter > 1) {
            objectHasLight = true;
        }
    } else {
        if (lightCounter >= blackCounter && size > 0) {
            objectHasLight = true;
        }
    }

    return objectHasLight;
}

/*
 * For parameter totalLight return pair that contains the min values for light (first)
 * and min values for black (second)
 */
pair<int, int> ActualDetector::checkBrightness(int totalLight) {
    //first int = minLight
    //second int = minBlack
    if (totalLight < 10) {
        return make_pair(50, 30);
    } else if (totalLight < 70) {
        return make_pair(90, 75);
    } else if (totalLight < 100) {
        return make_pair(110, 85);
    } else if (totalLight < 130) {
        return make_pair(150, 130);
    } else if (totalLight < 150) {
        return make_pair(160, 145);
    } else if (totalLight < 180) {
        return make_pair(190, 170);
    } else if (totalLight < 210) {
        return make_pair(215, 205);
    } else if (totalLight < 230) {
        return make_pair(235, 225);
    } else if (totalLight < 241) {
        return make_pair(243, 235);
    } else {
        return make_pair(250, 240);
    }

}


bool ActualDetector::checkIfBird() {
    std::vector<Rect> birds;
    m_birdsCascade.detectMultiScale(m_croppedImageGray, birds, 1.1, 2, 0 | CASCADE_SCALE_IMAGE,
                                    Size(CLASSIFIER_DIMENSION_SIZE, CLASSIFIER_DIMENSION_SIZE));

    return (!birds.empty());
}


/*
 * Save image
 */
void ActualDetector::saveImg(string path, Mat &image) {
    stringstream ss;
    ss << path << m_imageCount << m_savedImageExtension;
    imwrite(ss.str(), image);
}

/*
 * Stop the Detection process
 */
void ActualDetector::stopThread() {
    m_region.clear();
    m_isMainThreadRunning = false;
    m_recorder->stopRecording(true, std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    if (m_mainThread) {
        m_mainThread->join();
        m_mainThread.reset();
        this_thread::sleep_for(chrono::seconds(1));
        m_nightCheckerThread->join();
        m_nightCheckerThread.reset();
    }
}

bool ActualDetector::start() {
    if (!m_mainThread) {
        emit progressValueChanged(1);
        if (initialize()) {
            m_isMainThreadRunning = true;
            emit progressValueChanged(90);
            this_thread::sleep_for(chrono::seconds(1));
            emit progressValueChanged(100);

            if (!m_mainThread) {
                m_mainThread = std::make_unique<std::thread>(&ActualDetector::detectingThread, this);
            }
            return true;
        } else return false;
    }
    return true;
}

void ActualDetector::setNoiseLevel(int level) {
    m_noiseLevel = getStructuringElement(MORPH_RECT, Size(level, level));
}

void ActualDetector::setThresholdLevel(int level) {
    m_thresholdLevel = level;
}

Recorder *ActualDetector::getRecorder() {
    return m_recorder.get();
}

void ActualDetector::setShowCameraVideo(bool show) {
    m_showCameraVideo = show;
}

void ActualDetector::recordingFinished() {
    qDebug() << "ActualDetector::recordingFinished()";
    m_startedRecording = false;
}

