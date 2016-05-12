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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QTime>
#include <opencv2/imgproc/imgproc.hpp>
#include <QMessageBox>
#include <QDebug>
#include "clickablelabel.h"
#include "videowidget.h"
#include "camera.h"
#include "settingsdialog.h"
#include "imageexplorer.h"
#include <QDesktopServices>
#include <QDir>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QByteArray>
#include "uploader.h"
#include <QJsonDocument>
#include <QJsonObject>

using namespace cv;

MainWindow::MainWindow(QWidget *parent, Camera *cameraPtr, Config *configPtr) :
    QMainWindow(parent), ui(new Ui::MainWindow), CamPtr(cameraPtr)
{
    ui->setupUi(this);
    qDebug() << "begin constructing mainwindow";

    QWidget* mainWindowCentralWidget = new QWidget(this);
    mainWindowCentralWidget->setLayout(ui->mainWindowLayout);
    setCentralWidget(mainWindowCentralWidget);

    m_config = configPtr;

    programVersion = "0.6.1";

    this->setWindowTitle("UFO Detector | BETA " + programVersion);

    const int WIDTH = m_config->cameraWidth();
    const int HEIGHT = m_config->cameraHeight();
    const int NOISELEVEL = m_config->noiseFilterPixelSize();
    QString VERSION = m_config->applicationVersion();
    const int CODEC = m_config->resultVideoCodec();

    if ((VERSION == "") || (VERSION < programVersion))
    {
        m_config->resetDetectionAreaFile();
        qDebug() << "cleared critical settings";
        m_config->setApplicationVersion(programVersion);
    }

    ui->sliderNoise->setValue(NOISELEVEL);
    ui->lineNoise->setText(QString::number(NOISELEVEL));
    ui->sliderThresh->setValue(m_config->motionThreshold());
    ui->lineThresh->setText(QString::number(m_config->motionThreshold()));
    isUpdating=true;
    isDetecting=false;
    isRecording=false;
    lastWasPositive=false;
    lastWasInfo=true;
    counterNegative_=0;
    counterPositive_=0;
    recordingCounter_=0;

    m_detectionStatusStyleOn = "color: #55FF55;";
    m_detectionStatusStyleOff = "color: #FF5555;";

    checkFolders();
    readLogFileAndGetRootElement();
    checkDetectionAreaFile();

    ui->statusLabel->setStyleSheet(m_detectionStatusStyleOff);
    ui->recordingTestButton->hide();
    ui->progressBar->hide();
    ui->outputText->append("For feedback and bug reports contact the developer at contact@ufoid.net");

    if (checkCameraAndCodec(WIDTH,HEIGHT,CODEC))
    {
        threadWebcam.reset(new std::thread(&MainWindow::updateWebcamFrame, this));
    }


    //Add VideoWidget to UI
    QDomNode node = documentXML.firstChildElement().firstChild();
    while( !node.isNull())
    {
        if( node.isElement())
        {
            QDomElement element = node.toElement();
            VideoWidget* mytest = new VideoWidget(this, element.attribute("Pathname", "NULL"), element.attribute("DateTime", "NULL"),element.attribute("Length", "NULL")  );
            connect(mytest->deleteButton(), SIGNAL(clicked()),this,SLOT(onVideoDeleteClicked()));
            connect(mytest->uploadButton(), SIGNAL(clicked()),this,SLOT(onVideoUploadClicked()));
            connect(mytest->playButton(), SIGNAL(clicked()),this,SLOT(onVideoPlayClicked()));
            QListWidgetItem* item = new QListWidgetItem;
            item->setSizeHint(QSize(150,100));
            ui->videoList->addItem(item);
            ui->videoList->setItemWidget(item,mytest);
        }
        node = node.nextSibling();
    }

    //Check for new version
    if (m_config->checkApplicationUpdates())
    {
        manager = new QNetworkAccessManager();
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkForUpdate(QNetworkReply*)) );
        QNetworkRequest request;
        request.setUrl(QUrl("http://ufoid.net/version.xml"));
        request.setRawHeader( "User-Agent" , "Mozilla Firefox" );
        manager->get(request);
    }

    qDebug() << "mainwindow constructed" ;
}


/*
 * Read webcamframe from webcam
 */
void MainWindow::updateWebcamFrame()
{
    cv::Mat resizedFrame;
    while (isUpdating)
    {
        webcamFrame = CamPtr->getWebcamFrame();
        cv::resize(webcamFrame,resizedFrame, displayResolution,0, 0, INTER_CUBIC);
        cv::cvtColor(resizedFrame, resizedFrame, CV_BGR2RGB);
        QImage qWebcam((uchar*)resizedFrame.data, resizedFrame.cols, resizedFrame.rows, resizedFrame.step, QImage::Format_RGB888);
        emit updatePixmap(qWebcam);
    }
}

void MainWindow::displayPixmap(QImage image)
{
    ui->webcamView->setPixmap(QPixmap::fromImage(image));
}

/*
 * Set the signals and slots
 */
void MainWindow::setSignalsAndSlots(ActualDetector* ptrDetector)
{
    theDetector = ptrDetector;
    connect(theDetector,SIGNAL(positiveMessage()),this,SLOT(setPositiveMessage()));
    connect(theDetector,SIGNAL(negativeMessage()),this,SLOT(setNegativeMessage()));
    connect(theDetector,SIGNAL(errorReadingDetectionAreaFile()),this,SLOT(setErrorReadingDetectionAreaFile()));
    connect(theDetector->getRecorder(),SIGNAL(updateListWidget(QString,QString,QString)),this,SLOT(updateWidgets(QString,QString,QString)));
    connect(theDetector->getRecorder(),SIGNAL(recordingStarted()),this,SLOT(recordingWasStarted()));
    connect(theDetector->getRecorder(),SIGNAL(recordingStopped()),this,SLOT(recordingWasStoped()));
    connect(this,SIGNAL(elementWasRemoved()),theDetector->getRecorder(),SLOT(reloadResultDataFile()));
    connect(theDetector, SIGNAL(progressValueChanged(int)), this , SLOT(on_progressBar_valueChanged(int)) );
    connect(theDetector, SIGNAL(broadcastOutputText(QString)), this, SLOT(update_output_text(QString)) );
    connect(this,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
}

/*
 * Add new VideoWidget element
 */
void MainWindow::updateWidgets(QString filename, QString datetime, QString videoLength)
{
    VideoWidget* newWidget = new VideoWidget(this, filename, datetime, videoLength);
    connect(newWidget->deleteButton(), SIGNAL(clicked()),this,SLOT(onVideoDeleteClicked()));
    connect(newWidget->uploadButton(), SIGNAL(clicked()),this,SLOT(onVideoUploadClicked()));
    connect(newWidget->playButton(), SIGNAL(clicked()),this,SLOT(onVideoPlayClicked()));
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(150,100));
    ui->videoList->addItem(item);
    ui->videoList->setItemWidget(item,newWidget);

    recordingCounter_++;
    ui->lineCount->setText(QString::number(recordingCounter_));
    readLogFileAndGetRootElement();
}

/*
 * Play a video from the VideoWidget
 */
void MainWindow::onVideoPlayClicked()
{
    if(!isRecording){
        for(int row = 0; row < ui->videoList->count(); row++)
        {
            QListWidgetItem *item = ui->videoList->item(row);
            VideoWidget* widget = qobject_cast<VideoWidget*>(ui->videoList->itemWidget(item));

            if(widget->playButton()==sender())
            {
                QDesktopServices::openUrl(QUrl::fromUserInput(widget->videoFileName()));
            }
        }
    }
    else
    {
        QMessageBox::warning(this,"Error","Playing video file while recording another video is disabled. Wait until the recording has finished");
    }
}

/*
 * Delete video file and remove VideoWidget element from list
 */
void MainWindow::onVideoDeleteClicked()
{
    QString dateToRemove;
    for(int row = 0; row < ui->videoList->count(); row++)
    {
        QListWidgetItem *item = ui->videoList->item(row);
        VideoWidget* widget = qobject_cast<VideoWidget*>(ui->videoList->itemWidget(item));

        if(widget->deleteButton()==sender())
        {
            dateToRemove = widget->dateTime();
            QListWidgetItem *itemToRemove = ui->videoList->takeItem(row);
            ui->videoList->removeItemWidget(itemToRemove);
            qDebug() << "Removing" << widget->videoFileName() << "and its thumbnail";
            QFile::remove(widget->videoFileName());
            QFile::remove(widget->thumbnailFileName());
        }
    }

    QDomNode node = documentXML.firstChildElement().firstChild();
    while( !node.isNull())
    {
        if( node.isElement())
        {
            QDomElement element = node.toElement();
            QString dateInXML = element.attribute("DateTime");
            if (dateInXML.compare(dateToRemove)==0)
            {
              documentXML.firstChildElement().removeChild(node);
              if(!logFile.open(QIODevice::WriteOnly | QIODevice::Text))
              {
                  qDebug() <<  "fail open xml - delete";
                  return;
              }
              else
              {
                  QTextStream stream(&logFile);
                  stream.setCodec("UTF-8");
                  stream << documentXML.toString();
                  logFile.close();
              }
              break;
            }
        }
        node = node.nextSibling();
    }
    logFile.close();
    emit elementWasRemoved();
}

/*
 * Display the Upload Window
 */
void MainWindow::onVideoUploadClicked()
{
    for(int row = 0; row < ui->videoList->count(); row++)
    {
        QListWidgetItem *item = ui->videoList->item(row);
        VideoWidget* widget = qobject_cast<VideoWidget*>(ui->videoList->itemWidget(item));
        if(widget->uploadButton()==sender())
        {
            Uploader* upload = new Uploader(this,widget->videoFileName(),m_config);
            upload->show();
            upload->setAttribute(Qt::WA_DeleteOnClose);
        }
    }
}

/*
 * Set positive message from ActualDetector
 */
void MainWindow::setPositiveMessage()
{
    QTime time = QTime::currentTime();
    QString message = time.toString();

    if (!lastWasInfo)
    {
        ui->outputText->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
        ui->outputText->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
        ui->outputText->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
        ui->outputText->textCursor().removeSelectedText();
        ui->outputText->textCursor().deletePreviousChar();
        ui->outputText->append(message + " - " + "Positive: " + QString::number(++counterPositive_) + " Negative: " + QString::number(counterNegative_));
    }
    else ui->outputText->append(message + " - " + "Positive detection " + QString::number(++counterPositive_));
    lastWasPositive=true;
    lastWasInfo=false;
}

/*
 * Set negative message from ActualDetector
 */
void MainWindow::setNegativeMessage()
{
    QTime time = QTime::currentTime();
    QString message = time.toString();

    if (!lastWasInfo)
    {
        ui->outputText->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
        ui->outputText->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
        ui->outputText->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
        ui->outputText->textCursor().removeSelectedText();
        ui->outputText->textCursor().deletePreviousChar();
        ui->outputText->append(message + " - " + "Positive: " + QString::number(counterPositive_) + " Negative: " + QString::number(++counterNegative_));
    }
    else ui->outputText->append(message + " - " + "Negative detection " + QString::number(++counterNegative_));
    lastWasPositive=false;
    lastWasInfo=false;
}

/*
 * Error when reading detection area file
 */
void MainWindow::setErrorReadingDetectionAreaFile()
{
    ui->outputText->append("ERROR: xml file containing the detection area information was not read correctly");
    ui->outputText->append("ERROR: Detection is not working. Select area of detection first in the settings.");
}

/*
 * Set messages from ActualDetector
 */
void MainWindow::addOutputText(QString msg)
{
    QTime time = QTime::currentTime();
    QString output_text(time.toString() + " - " + msg);
    ui->outputText->append(output_text);
    lastWasInfo=true;
    counterNegative_=0;
    counterPositive_=0;
}


void MainWindow::on_sliderNoise_sliderMoved(int position)
{
    ui->lineNoise->setText(QString::number(position));
    theDetector->setNoiseLevel(position);
}

void MainWindow::on_sliderThresh_sliderMoved(int position)
{
    ui->lineThresh->setText(QString::number(position));
    theDetector->setThresholdLevel(position);
}

void MainWindow::on_settingsButton_clicked()
{
    if (!isDetecting)
    {
        settingsDialog = new SettingsDialog(0, CamPtr, m_config);
        settingsDialog->setModal(true);
        settingsDialog->show();
        settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        QMessageBox::warning(this,"Error","Stop the detection process first");
    }
}


void MainWindow::on_startButton_clicked()
{
    if (!isDetecting)
    {
        ui->progressBar->show();
        ui->progressBar->repaint();

        disconnect(this,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
        if(theDetector->start())
        {
            theDetector->setNoiseLevel(ui->sliderNoise->value());
            theDetector->setThresholdLevel(ui->sliderThresh->value());
            isUpdating = false;
            if (threadWebcam)
            {
                threadWebcam->join(); threadWebcam.reset();
            }
            connect(theDetector,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
            isDetecting=true;
            ui->statusLabel->setStyleSheet(m_detectionStatusStyleOn);
            ui->statusLabel->setText("Detection started on " + QTime::currentTime().toString());
            ui->progressBar->hide();
            ui->startButton->setText("Stop Detection");
        }
        else
        {
            ui->statusLabel->setText("Failed to start");
            connect(this,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
        }
    }
    else
    {
        on_stopButton_clicked();
    }
}

void MainWindow::on_stopButton_clicked()
{
    theDetector->stopThread();
    ui->statusLabel->setStyleSheet(m_detectionStatusStyleOff);
    ui->statusLabel->setText("Detection not running");
    isDetecting=false;
    disconnect(theDetector,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
    connect(this,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
    if (!threadWebcam)
    {
        isUpdating=true;
        threadWebcam.reset(new std::thread(&MainWindow::updateWebcamFrame, this));
    }
    ui->startButton->setText("Start Detection");
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1==0){theDetector->willDisplayImage = false;}
    if(arg1==2){theDetector->willDisplayImage = true;}
}

void MainWindow::on_buttonClear_clicked()
{
    ui->outputText->clear();
}


void MainWindow::on_recordingTestButton_clicked()
{
    theDetector->startRecording();
}

void MainWindow::on_aboutButton_clicked()
{
    QMessageBox::information(this,"About",QString("UFO Detector | Beta " + programVersion + "\nwww.UFOID.net \ncontact@ufoid.net"));
}

void MainWindow::on_buttonImageExpl_clicked()
{
    ImageExplorer* imageExpl = new ImageExplorer(0,m_config);
    imageExpl->setModal(true);
    imageExpl->show();
    imageExpl->setAttribute(Qt::WA_DeleteOnClose);
}

/*
 * Check resolution and change UI to fit around the webcam frame
 * return false if incorrect aspect ratio
 */
bool MainWindow::checkAndSetResolution(const int WIDTH, const int HEIGHT)
{
    double aspectRatio = (double)WIDTH / (double)HEIGHT;
    /// @todo use actual maximum web camera view width AND height -- main window is resizable so view size changes
    int maxWebcamWidth = 640;
    int webcamHeight = (int)(maxWebcamWidth / aspectRatio);
    bool aspectRatioOk = false;

    qDebug() << "Requested web cam size:" << QSize(WIDTH, HEIGHT);
    qDebug() << "Requested aspect ratio of web camera:" << aspectRatio;
    if (m_config->checkCameraAspectRatio())
    {
        if (CamPtr->knownAspectRatios().contains((int)(aspectRatio * 10000)))
        {
            aspectRatioOk = true;
        }
    } else {
        aspectRatioOk = true;
    }
    if (aspectRatioOk)
    {
        qDebug() << "Aspect ratio of web camera is ok";
        ui->webcamView->resize(QSize(maxWebcamWidth, webcamHeight));
        displayResolution = Size(maxWebcamWidth, webcamHeight);
        return true;
    }
    qDebug() << "Aspect ratio of web camera is NOT ok";
    ui->outputText->append("Error: Selected webcam resolution is NOT ok");
    return false;
}


/*
 * Get the DisplayImages checkbox state for the ActualDetector
 */
bool MainWindow::getCheckboxState()
{
    return ui->checkBox->isChecked();
}

void MainWindow::readLogFileAndGetRootElement()
{
    logFile.setFileName(m_config->resultDataFile());
    if(logFile.exists())
    {
        if(!logFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "failed to read the result data file" << endl;
        }
        else
        {
            if(!documentXML.setContent(&logFile))
            {
                qDebug() << "failed to load the result data file" << endl;
            }
            else
            {
                logFile.close();
                qDebug() << "correctly loaded root element from result data file" << endl;
            }
        }
    }
    else
    {
        if(!logFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "failed creating result data file" << endl;
        }
        else
        {
            qDebug() << "creating result data file" << endl;
            QDomDocument tempFirstTime;
            tempFirstTime.appendChild(tempFirstTime.createElement("UFOID"));
            QTextStream stream(&logFile);
            stream << tempFirstTime.toString();
            logFile.close();
            readLogFileAndGetRootElement();
        }
    }
}

void MainWindow::checkDetectionAreaFile()
{
    QString areaFileName = m_config->detectionAreaFile();
    QFile areaFile(areaFileName);
    if(areaFile.exists())
    {
         qDebug() << "found detection area file";
    }
    else
    {
        areaFile.setFileName(areaFileName);
        if (areaFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "created detection area file " << areaFile.fileName();
            areaFile.close();
        } else {
            qDebug() << "failed to create detection area file " << areaFile.fileName();
        }
    }
}

/*
 * Check camera and codec
 */
bool MainWindow::checkCameraAndCodec(const int WIDTH, const int HEIGHT, const int CODEC)
{
    bool success = false;
    if (checkAndSetResolution(WIDTH,HEIGHT)&&!threadWebcam&&CamPtr->isWebcamOpen())
    {
        try
        {
            webcamFrame = CamPtr->getWebcamFrame();
            cv::resize(webcamFrame,webcamFrame, displayResolution,0, 0, INTER_CUBIC);
            success = true;
        }
        catch( cv::Exception& e )
        {
            const char* err_msg = e.what();
            std::cout << "exception caught: " << err_msg << std::endl;
            CamPtr->stopReadingWebcam();
            ui->outputText->append("ERROR: Found webcam but video frame could not be read. Reconnect and check resolution in settings");
        }
    }
    else if (!CamPtr->isWebcamOpen())
    {
        ui->outputText->append("ERROR: Could not open webcam. Select webcam in settings");
    }


    if (CODEC==0)
    {
        VideoWriter theVideoWriter;
        theVideoWriter.open("filename.avi",0, 25,Size(WIDTH,HEIGHT), true);
        if (!theVideoWriter.isOpened())
        {
            ui->outputText->append("ERROR: Could not find Raw Video Codec");
            ui->outputText->append("ERROR: Please contact the developer with the information about your system");
        }
        else
        {
            theVideoWriter.release();
            remove("filename.avi");
        }
    }
    else if (CODEC==1)
    {
        VideoWriter theVideoWriter;
        theVideoWriter.open("filename.avi",0, 25,Size(WIDTH,HEIGHT), true);
        if (!theVideoWriter.isOpened())
        {
            ui->outputText->append("ERROR: Could not find Raw Video Codec");
            ui->outputText->append("ERROR: Please contact the developer with the information about your system");
        }
        else
        {
            theVideoWriter.release();
            remove("filename.avi");
        }

        QFile ffmpegFile(m_config->videoEncoderLocation());
        if(!ffmpegFile.exists())
        {
            ui->outputText->append("ERROR: Could not find video encoder needed for FFV1 Codec.");
            ui->outputText->append("ERROR: Please install ffmpeg or avconv, or contact the developer.");
        } else {
            ui->outputText->append("Using video encoder "+m_config->videoEncoderLocation());
        }

    }
    else if (CODEC==2)
    {
        VideoWriter theVideoWriter;
        theVideoWriter.open("filename.avi",CV_FOURCC('L','A','G','S'), 25,Size(WIDTH,HEIGHT), true);
        if (!theVideoWriter.isOpened())
        {
            ui->outputText->append("ERROR: Could not find Lagarith Lossless Video Codec");
            ui->outputText->append("ERROR: Download and install from http://lags.leetcode.net/codec.html");
        }
        else
        {
            theVideoWriter.release();
            remove("filename.avi");
        }

    }

    ui->startButton->setEnabled(success);

    return success;
}

void MainWindow::checkFolders()
{
    QDir videoFolder(m_config->resultVideoDir());
    QFileInfo detectionAreaFileInfo(m_config->detectionAreaFile());
    QFileInfo resultDataFileInfo(m_config->resultDataFile());
    QFileInfo birdTrainingDataFileInfo(m_config->birdClassifierTrainingFile());
    QDir detectionAreaFileFolder(detectionAreaFileInfo.absoluteDir());
    QDir resultDataFileFolder(resultDataFileInfo.absoluteDir());
    QDir birdTrainingDataFileFolder(birdTrainingDataFileInfo.absoluteDir());

    if (!(videoFolder.exists() && !m_config->resultVideoDir().isEmpty()))
    {
        qDebug() << "creating video and image folders";
        videoFolder.mkpath(m_config->resultImageDir());
        videoFolder.mkpath(m_config->resultVideoDir());
    }

    if (!detectionAreaFileFolder.exists() && !detectionAreaFileFolder.absolutePath().isEmpty()) {
        qDebug() << "Creating folder for detection area file:" << detectionAreaFileFolder.absolutePath();
        detectionAreaFileFolder.mkpath(detectionAreaFileFolder.absolutePath());
    }

    if (!resultDataFileFolder.exists() && !resultDataFileFolder.absolutePath().isEmpty()) {
        qDebug() << "Creating folder for result data file:" << resultDataFileFolder.absolutePath();
        resultDataFileFolder.mkpath(resultDataFileFolder.absolutePath());
    }

    if (!birdTrainingDataFileFolder.exists() && !birdTrainingDataFileFolder.absolutePath().isEmpty()) {
        qDebug() << "Creating folder for bird classifier training data file:" << birdTrainingDataFileFolder.absolutePath();
        birdTrainingDataFileFolder.mkpath(birdTrainingDataFileFolder.absolutePath());
    }
}

/*
 * Check if xml indicates that there is a new version of the program. Validate userToken if necessary
 */
void MainWindow::checkForUpdate(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    qDebug() <<   "Downloaded version data," << data.size() << "bytes";
    QDomDocument versionXML;

    if(!versionXML.setContent(data))
    {
        qWarning() << "Failed to parse downloaded version data";
    }
    else
    {
        QDomElement root;
        root=versionXML.firstChildElement();
        QString versionInXML;
        std::queue<QString> messageInXML;

        QDomNode node = root.firstChild();
        while( !node.isNull())
        {
            if( node.isElement())
            {
                if(node.nodeName()=="version")
                {
                    QDomElement element = node.toElement();
                    versionInXML=element.text();
                }
                if(node.nodeName()=="message")
                {
                    QDomElement element = node.toElement();
                    messageInXML.push(element.text());
                }
            }
            node = node.nextSibling();
        }

        if(versionInXML>programVersion)
        {
            qDebug() << messageInXML.size();
            updateWindow = new MessageUpdate(this,versionInXML,messageInXML);
            updateWindow->show();
            updateWindow->setAttribute(Qt::WA_DeleteOnClose);
        }
    }

    delete reply;
    reply = nullptr;

}

void MainWindow::recordingWasStarted()
{
    isRecording=true;
}

void MainWindow::recordingWasStoped()
{
    isRecording=false;
}

MainWindow::~MainWindow()
{
    qDebug() << "Deconstructing MainWindow" ;
    delete ui;
}

/*
 * Stop detecting and save settings when closing application
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    theDetector->stopThread();

    m_config->setNoiseFilterPixelSize(ui->sliderNoise->value());
    m_config->setMotionThreshold(ui->sliderThresh->value());
    isUpdating = false;
    if (threadWebcam)
    {
        threadWebcam->join();
        threadWebcam.reset();
    }
    CamPtr->stopReadingWebcam();
    QApplication::quit();
}

void MainWindow::on_progressBar_valueChanged(int value)
{
    ui->progressBar->setValue(value);
}

void MainWindow::update_output_text(QString msg)
{
    addOutputText(msg);
}

void MainWindow::on_toolButtonNoise_clicked()
{
    QMessageBox::information(this,"Information","Select the pixel size of noise that will be ignored in the detection. \nRecommended selection: 2");
}

void MainWindow::on_toolButtonThresh_clicked()
{
    QMessageBox::information(this,"Information","Select the threshold filter value that will be used in the motion detection algorithm. \nIncrease the value if clouds are being detected\nRecommended selection: 10");
}

