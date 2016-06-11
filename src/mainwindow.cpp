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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Camera *cameraPtr, Config *configPtr) :
    QMainWindow(parent), ui(new Ui::MainWindow), m_camera(cameraPtr)
{
    ui->setupUi(this);
    qDebug() << "Constructing MainWindow";

    QWidget* mainWindowCentralWidget = new QWidget(this);
    mainWindowCentralWidget->setLayout(ui->mainWindowLayout);
    setCentralWidget(mainWindowCentralWidget);

    m_config = configPtr;

    m_programVersion = "0.6.4";

    this->setWindowTitle("UFO Detector | BETA " + m_programVersion);

    const int width = m_config->cameraWidth();
    const int height = m_config->cameraHeight();
    m_cameraViewResolution = Size(width, height);
    const int NOISELEVEL = m_config->noiseFilterPixelSize();
    QString VERSION = m_config->applicationVersion();
    const int codec = m_config->resultVideoCodec();

    if ((VERSION == "") || (VERSION < m_programVersion))
    {
        m_config->resetDetectionAreaFile();
        qDebug() << "cleared critical settings";
        m_config->setApplicationVersion(m_programVersion);
    }

    ui->sliderNoise->setValue(NOISELEVEL);
    ui->lineNoise->setText(QString::number(NOISELEVEL));
    ui->sliderThresh->setValue(m_config->motionThreshold());
    ui->lineThresh->setText(QString::number(m_config->motionThreshold()));
    m_showCameraVideo=true;
    m_detecting=false;
    m_recordingVideo=false;
    m_lastMessageWasInfo=true;
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
    ui->outputText->append(tr("For feedback and bug reports contact the developer at contact@ufoid.net"));

    if (checkCameraAndCodec(width, height, codec))
    {
        threadWebcam.reset(new std::thread(&MainWindow::updateWebcamFrame, this));
    }

    //Add VideoWidget to UI
    QDomNode node = m_resultDataDomDocument.firstChildElement().firstChild();
    while( !node.isNull())
    {
        if( node.isElement())
        {
            QDomElement element = node.toElement();
            VideoWidget* videoWidget = new VideoWidget(this, element.attribute("Pathname", "NULL"), element.attribute("DateTime", "NULL"),element.attribute("Length", "NULL")  );
            connect(videoWidget->deleteButton(), SIGNAL(clicked()),this,SLOT(onVideoDeleteClicked()));
            connect(videoWidget->uploadButton(), SIGNAL(clicked()),this,SLOT(onVideoUploadClicked()));
            connect(videoWidget->playButton(), SIGNAL(clicked()),this,SLOT(onVideoPlayClicked()));
            QListWidgetItem* item = new QListWidgetItem;
            item->setSizeHint(QSize(150,100));
            ui->videoList->addItem(item);
            ui->videoList->setItemWidget(item,videoWidget);
        }
        node = node.nextSibling();
    }

    //Check for new version
    if (m_config->checkApplicationUpdates())
    {
        m_networkAccessManager = new QNetworkAccessManager();
        connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkForUpdate(QNetworkReply*)) );
        QNetworkRequest request;
        request.setUrl(QUrl("http://ufoid.net/version.xml"));
        request.setRawHeader( "User-Agent" , "Mozilla Firefox" );
        m_networkAccessManager->get(request);
    }
    qDebug() << "MainWindow constructed" ;
}


/*
 * Read webcamframe from webcam
 */
void MainWindow::updateWebcamFrame()
{
    cv::Mat resizedFrame;
    while (m_showCameraVideo)
    {
        m_webcamFrame = m_camera->getWebcamFrame();
        /// @todo sync because displayResolution could change at any moment
        cv::resize(m_webcamFrame,resizedFrame, m_cameraViewResolution,0, 0, INTER_CUBIC);
        cv::cvtColor(resizedFrame, resizedFrame, CV_BGR2RGB);
        m_cameraViewImageMutex.lock();
        m_cameraViewImage = QImage((uchar*)resizedFrame.data, resizedFrame.cols, resizedFrame.rows, resizedFrame.step, QImage::Format_RGB888);
        m_cameraViewImageMutex.unlock();
        emit updatePixmap(m_cameraViewImage);
        std::this_thread::yield();
    }
}

void MainWindow::displayPixmap(QImage image)
{
    m_cameraViewImageMutex.lock();
    ui->cameraView->setPixmap(QPixmap::fromImage(image));
    m_cameraViewImageMutex.unlock();
}

/*
 * Set the signals and slots
 */
void MainWindow::setSignalsAndSlots(ActualDetector* actualDetector)
{
    m_actualDetector = actualDetector;
    connect(m_actualDetector, SIGNAL(positiveMessage()), this, SLOT(setPositiveMessage()));
    connect(m_actualDetector, SIGNAL(negativeMessage()), this, SLOT(setNegativeMessage()));
    connect(m_actualDetector, SIGNAL(errorReadingDetectionAreaFile()), this, SLOT(setErrorReadingDetectionAreaFile()));
    connect(m_actualDetector->getRecorder(), SIGNAL(resultDataSaved(QString,QString,QString)), this, SLOT(addVideoToVideoList(QString,QString,QString)));
    connect(m_actualDetector->getRecorder(), SIGNAL(recordingStarted()), this, SLOT(onRecordingStarted()));
    connect(m_actualDetector->getRecorder(), SIGNAL(recordingFinished()), this, SLOT(onRecordingFinished()));
    connect(this, SIGNAL(elementWasRemoved()), m_actualDetector->getRecorder(), SLOT(reloadResultDataFile()));
    connect(m_actualDetector, SIGNAL(progressValueChanged(int)), this, SLOT(on_progressBar_valueChanged(int)));
    connect(m_actualDetector, SIGNAL(broadcastOutputText(QString)), this, SLOT(update_output_text(QString)));
    connect(this, SIGNAL(cameraViewFrameSizeChanged(QSize)), m_actualDetector, SLOT(onCameraViewFrameSizeChanged(QSize)));
    connect(this, SIGNAL(updatePixmap(QImage)), this, SLOT(displayPixmap(QImage)));
    connect(ui->videoList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onVideoListContextMenuRequested(const QPoint&)));
}

/*
 * Add new VideoWidget element to video list
 */
void MainWindow::addVideoToVideoList(QString filename, QString datetime, QString videoLength)
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
    if(!m_recordingVideo){
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
        QMessageBox::warning(this, tr("Error"), tr("Playing video file while recording another video is disabled. Wait until the recording has finished."));
    }
}

/*
 * Delete button on VideoWidget was clicked: delete the video
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
            int ret = QMessageBox::warning(this, tr("Delete video"),
                tr("Do you want to permanently delete this video?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (QMessageBox::Yes == ret)
            {
                this->removeVideo(dateToRemove);
            }
        }
    }
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
            VideoUploaderDialog* upload = new VideoUploaderDialog(this,widget->videoFileName(),m_config);
            upload->show();
            upload->setAttribute(Qt::WA_DeleteOnClose);
        }
    }
}

void MainWindow::onVideoListContextMenuRequested(const QPoint& pos)
{
    Q_UNUSED(pos);
    QMenu contextMenu(tr("Video list"), this);
    QAction* deleteSelectedItemsAction = contextMenu.addAction(tr("Delete selected items"));
    connect(deleteSelectedItemsAction, SIGNAL(triggered(bool)), this, SLOT(onDeleteSelectedVideosClicked()));
    if (ui->videoList->selectedItems().isEmpty())
    {
        deleteSelectedItemsAction->setEnabled(false);
    }
    contextMenu.exec(QCursor::pos());
}

void MainWindow::onDeleteSelectedVideosClicked()
{
    int ret = QMessageBox::warning(this, tr("Delete videos"),
        tr("Do you really want to delete the selected videos?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (QMessageBox::Yes == ret)
    {
        QList<QListWidgetItem*> itemList = ui->videoList->selectedItems();
        QListIterator<QListWidgetItem*> itemIt(itemList);
        while (itemIt.hasNext())
        {
            QListWidgetItem* item = itemIt.next();
            VideoWidget* widget = qobject_cast<VideoWidget*>(ui->videoList->itemWidget(item));
            this->removeVideo(widget->dateTime());
        }
    }
}

/*
 * Set positive message from ActualDetector
 */
void MainWindow::setPositiveMessage()
{
    QTime time = QTime::currentTime();
    QString timestamp = time.toString();

    if (!m_lastMessageWasInfo)
    {
        ui->outputText->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
        ui->outputText->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
        ui->outputText->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
        ui->outputText->textCursor().removeSelectedText();
        ui->outputText->textCursor().deletePreviousChar();
        ui->outputText->append(tr("%1 - Positive: %2 Negative: %3").arg(
            timestamp, QString::number(++counterPositive_), QString::number(counterNegative_)));
    }
    else ui->outputText->append(tr("%1 - Positive detections: %2").arg(timestamp, QString::number(++counterPositive_)));
    m_lastMessageWasInfo=false;
}

/*
 * Set negative message from ActualDetector
 */
void MainWindow::setNegativeMessage()
{
    QTime time = QTime::currentTime();
    QString timestamp = time.toString();

    if (!m_lastMessageWasInfo)
    {
        ui->outputText->moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
        ui->outputText->moveCursor( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
        ui->outputText->moveCursor( QTextCursor::End, QTextCursor::KeepAnchor );
        ui->outputText->textCursor().removeSelectedText();
        ui->outputText->textCursor().deletePreviousChar();
        ui->outputText->append(tr("%1 - Positive: %2 Negative: %3").arg(
            timestamp, QString::number(counterPositive_), QString::number(++counterNegative_)));
    }
    else ui->outputText->append(tr("%1 - Negative detections: %2").arg(timestamp, QString::number(++counterNegative_)));
    m_lastMessageWasInfo=false;
}

/*
 * Error when reading detection area file
 */
void MainWindow::setErrorReadingDetectionAreaFile()
{
    ui->outputText->append(tr("ERROR: XML file containing the detection area information was not read correctly."));
    ui->outputText->append(tr("ERROR: Detection is not working. Select area of detection first in the settings."));
}

/*
 * Set messages from ActualDetector
 */
void MainWindow::addOutputText(QString msg)
{
    QTime time = QTime::currentTime();
    QString output_text(time.toString() + " - " + msg);
    ui->outputText->append(output_text);
    m_lastMessageWasInfo=true;
    counterNegative_=0;
    counterPositive_=0;
}


void MainWindow::on_sliderNoise_sliderMoved(int position)
{
    ui->lineNoise->setText(QString::number(position));
    m_actualDetector->setNoiseLevel(position);
}

void MainWindow::on_sliderThresh_sliderMoved(int position)
{
    ui->lineThresh->setText(QString::number(position));
    m_actualDetector->setThresholdLevel(position);
}

void MainWindow::on_settingsButton_clicked()
{
    if (!m_detecting)
    {
        m_settingsDialog = new SettingsDialog(0, m_camera, m_config);
        m_settingsDialog->setModal(true);
        m_settingsDialog->show();
        m_settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("Cannot edit settings when detection process is running. Stop the detection process first."));
    }
}


void MainWindow::on_startButton_clicked()
{
    if (!m_detecting)
    {
        ui->progressBar->show();
        ui->progressBar->repaint();

        disconnect(this,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
        if(m_actualDetector->start())
        {
            m_actualDetector->setNoiseLevel(ui->sliderNoise->value());
            m_actualDetector->setThresholdLevel(ui->sliderThresh->value());
            m_showCameraVideo = false;
            if (threadWebcam)
            {
                threadWebcam->join(); threadWebcam.reset();
            }
            connect(m_actualDetector,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
            m_detecting=true;
            ui->statusLabel->setStyleSheet(m_detectionStatusStyleOn);
            ui->statusLabel->setText(tr("Detection started at %1").arg(QTime::currentTime().toString()));
            ui->progressBar->hide();
            ui->startButton->setText(tr("Stop detection"));
        }
        else
        {
            ui->statusLabel->setText(tr("Failed to start detection"));
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
    m_actualDetector->stopThread();
    ui->statusLabel->setStyleSheet(m_detectionStatusStyleOff);
    ui->statusLabel->setText(tr("Detection not running"));
    m_detecting=false;
    disconnect(m_actualDetector,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
    connect(this,SIGNAL(updatePixmap(QImage)),this,SLOT(displayPixmap(QImage)));
    if (!threadWebcam)
    {
        m_showCameraVideo=true;
        threadWebcam.reset(new std::thread(&MainWindow::updateWebcamFrame, this));
    }
    ui->startButton->setText(tr("Start detection"));
}

void MainWindow::removeVideo(QString dateTime)
{
    for(int row = 0; row < ui->videoList->count(); row++)
    {
        QListWidgetItem *item = ui->videoList->item(row);
        VideoWidget* widget = qobject_cast<VideoWidget*>(ui->videoList->itemWidget(item));

        if(widget->dateTime() == dateTime)
        {
            QListWidgetItem *itemToRemove = ui->videoList->takeItem(row);
            ui->videoList->removeItemWidget(itemToRemove);
            qDebug() << "Removing" << widget->videoFileName() << "and its thumbnail";
            QFile::remove(widget->videoFileName());
            QFile::remove(widget->thumbnailFileName());
        }
    }

    QDomNode node = m_resultDataDomDocument.firstChildElement().firstChild();
    while( !node.isNull())
    {
        if( node.isElement())
        {
            QDomElement element = node.toElement();
            QString dateInXML = element.attribute("DateTime");
            if (dateInXML.compare(dateTime)==0)
            {
              m_resultDataDomDocument.firstChildElement().removeChild(node);
              if(!m_resultDataFile.open(QIODevice::WriteOnly | QIODevice::Text))
              {
                  qDebug() <<  "Failed to open result data file for item deletion";
                  return;
              }
              else
              {
                  QTextStream stream(&m_resultDataFile);
                  stream.setCodec("UTF-8");
                  stream << m_resultDataDomDocument.toString();
                  m_resultDataFile.close();
              }
              break;
            }
        }
        node = node.nextSibling();
    }
    emit elementWasRemoved();
}

void MainWindow::on_checkBoxDisplayWebcam_stateChanged(int arg1)
{
    if (Qt::Unchecked == arg1)
    {
        m_actualDetector->showCameraVideo(false);
    }
    else if (Qt::Checked == arg1)
    {
        m_actualDetector->showCameraVideo(true);
    }
}

void MainWindow::on_buttonClear_clicked()
{
    ui->outputText->clear();
}


void MainWindow::on_recordingTestButton_clicked()
{
    if (!m_recordingVideo)
    {
        m_actualDetector->startRecording();
        ui->recordingTestButton->setText("Stop rec");
    }
    else
    {
        m_actualDetector->getRecorder()->stopRecording(true);
        ui->recordingTestButton->setText("Start rec");
    }

}

void MainWindow::on_aboutButton_clicked()
{
    QMessageBox::information(this, tr("About"), QString("UFO Detector | Beta " + m_programVersion + "\nwww.UFOID.net\ncontact@ufoid.net"));
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
bool MainWindow::checkAndSetResolution(const int width, const int height)
{
    double aspectRatio = (double)width / (double)height;
    bool aspectRatioOk = false;

    qDebug() << "Requested web cam size:" << QSize(width, height);
    qDebug() << "Requested aspect ratio of web camera:" << aspectRatio;
    if (m_config->checkCameraAspectRatio())
    {
        if (m_camera->knownAspectRatios().contains((int)(aspectRatio * 10000)))
        {
            aspectRatioOk = true;
        }
    } else {
        aspectRatioOk = true;
    }
    if (aspectRatioOk)
    {
        adjustCameraViewFrameSize();
        return true;
    }
    qDebug() << "Aspect ratio of web camera is NOT ok";
    ui->outputText->append(tr("Error: Selected webcam resolution is NOT ok"));
    return false;
}

void MainWindow::adjustCameraViewFrameSize()
{
    QSize cameraFrameSize(m_config->cameraWidth(), m_config->cameraHeight());
    cameraFrameSize.scale(ui->cameraView->width(), ui->cameraView->height(), Qt::KeepAspectRatio);
    m_cameraViewResolution = Size(cameraFrameSize.width(), cameraFrameSize.height());
    // if image is not resized anywhere else do it here
    if (m_detecting && !ui->checkBoxDisplayWebcam->isChecked())
    {
        cv::Mat resizedFrame;
        cv::resize(m_webcamFrame, resizedFrame, m_cameraViewResolution, 0, 0, INTER_CUBIC);
        cv::cvtColor(resizedFrame, resizedFrame, CV_BGR2RGB);
        m_cameraViewImageMutex.lock();
        m_cameraViewImage = QImage((uchar*)resizedFrame.data, resizedFrame.cols, resizedFrame.rows, resizedFrame.step, QImage::Format_RGB888);
        m_cameraViewImageMutex.unlock();
        displayPixmap(m_cameraViewImage);   // slot
    }
    emit cameraViewFrameSizeChanged(cameraFrameSize);
}


/*
 * Get the DisplayImages checkbox state for the ActualDetector
 */
bool MainWindow::getCheckboxDisplayWebcamState()
{
    return ui->checkBoxDisplayWebcam->isChecked();
}

void MainWindow::readLogFileAndGetRootElement()
{
    /// @todo refactor result data file & DOM handling to a different class (e.g. ResultData)
    m_resultDataFile.setFileName(m_config->resultDataFile());
    if(m_resultDataFile.exists())
    {
        if(!m_resultDataFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "MainWindow: failed to read the result data file" << m_resultDataFile.fileName();
        }
        else
        {
            if(!m_resultDataDomDocument.setContent(&m_resultDataFile))
            {
                qDebug() << "MainWindow: failed to load the result data file" << m_resultDataFile.fileName();
            }
            else
            {
                qDebug() << "Correctly loaded root element from result data file";
            }
            m_resultDataFile.close();
        }
    }
    else
    {
        if(!m_resultDataFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Failed to create result data file" << m_resultDataFile.fileName();
        }
        else
        {
            qDebug() << "Creating result data file" << m_resultDataFile.fileName();
            QDomDocument tempFirstTime;
            tempFirstTime.appendChild(tempFirstTime.createElement("UFOID"));
            QTextStream stream(&m_resultDataFile);
            stream << tempFirstTime.toString();
            m_resultDataFile.close();
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
         qDebug() << "Found detection area file";
    }
    else
    {
        areaFile.setFileName(areaFileName);
        if (areaFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Created detection area file " << areaFile.fileName();
            areaFile.close();
        } else {
            qDebug() << "Failed to create detection area file " << areaFile.fileName();
        }
    }
}

/*
 * Check camera and codec
 */
bool MainWindow::checkCameraAndCodec(const int width, const int height, const int codec)
{
    bool success = false;
    if (checkAndSetResolution(width, height) && !threadWebcam && m_camera->isWebcamOpen())
    {
        try
        {
            m_webcamFrame = m_camera->getWebcamFrame();
            cv::resize(m_webcamFrame,m_webcamFrame, m_cameraViewResolution,0, 0, INTER_CUBIC);
            success = true;
        }
        catch( cv::Exception& e )
        {
            qDebug() << "Exception caught: " << QString(e.what());
            m_camera->stopReadingWebcam();
            ui->outputText->append(tr("ERROR: Found webcam but video frame could not be read. Reconnect and check resolution in settings."));
        }
    }
    else if (!m_camera->isWebcamOpen())
    {
        ui->outputText->append("ERROR: Could not open webcam. Select webcam in settings");
    }


    if (codec==0)
    {
        VideoWriter theVideoWriter;
        theVideoWriter.open("filename.avi", 0, 25, Size(width, height), true);
        if (!theVideoWriter.isOpened())
        {
            ui->outputText->append(tr("ERROR: Could not find Raw Video Codec"));
            ui->outputText->append(tr("ERROR: Please contact the developer with the information about your system"));
        }
        else
        {
            theVideoWriter.release();
            remove("filename.avi");
        }
    }
    else if (codec==1)
    {
        VideoWriter theVideoWriter;
        theVideoWriter.open("filename.avi",0, 25, Size(width, height), true);
        if (!theVideoWriter.isOpened())
        {
            ui->outputText->append(tr("ERROR: Could not find Raw Video Codec"));
            ui->outputText->append(tr("ERROR: Please contact the developer with the information about your system"));
        }
        else
        {
            theVideoWriter.release();
            remove("filename.avi");
        }

        QFile ffmpegFile(m_config->videoEncoderLocation());
        if(!ffmpegFile.exists())
        {
            ui->outputText->append(tr("ERROR: Could not find video encoder needed for FFV1 Codec."));
            ui->outputText->append(tr("ERROR: Please install ffmpeg or avconv, or contact the developer."));
        } else {
            ui->outputText->append(tr("Using video encoder %1").arg(m_config->videoEncoderLocation()));
        }

    }
    else if (codec==2)
    {
        VideoWriter theVideoWriter;
        theVideoWriter.open("filename.avi",CV_FOURCC('L','A','G','S'), 25, Size(width, height), true);
        if (!theVideoWriter.isOpened())
        {
            ui->outputText->append(tr("ERROR: Could not find Lagarith Lossless Video Codec"));
            ui->outputText->append(tr("ERROR: Download and install from http://lags.leetcode.net/codec.html"));
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

        if(versionInXML>m_programVersion)
        {
            qDebug() << messageInXML.size();
            m_updateApplicationDialog = new UpdateApplicationDialog(this, versionInXML, messageInXML);
            m_updateApplicationDialog->show();
            m_updateApplicationDialog->setAttribute(Qt::WA_DeleteOnClose);
        }
    }

    delete reply;
    reply = nullptr;

}

void MainWindow::onRecordingStarted()
{
    m_recordingVideo=true;
}

void MainWindow::onRecordingFinished()
{
    m_recordingVideo=false;
}

MainWindow::~MainWindow()
{
    qDebug() << "Deconstructing MainWindow" ;
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    adjustCameraViewFrameSize();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    adjustCameraViewFrameSize();
}

/*
 * Stop detecting and save settings when closing application
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    m_actualDetector->stopThread();

    m_config->setNoiseFilterPixelSize(ui->sliderNoise->value());
    m_config->setMotionThreshold(ui->sliderThresh->value());
    m_showCameraVideo = false;
    if (threadWebcam)
    {
        threadWebcam->join();
        threadWebcam.reset();
    }
    m_camera->stopReadingWebcam();
    m_camera->release();
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
    QMessageBox::information(this, tr("Information"), tr("Select the pixel size of noise that will be ignored in the detection. \nRecommended value: 2"));
}

void MainWindow::on_toolButtonThresh_clicked()
{
    QMessageBox::information(this, tr("Information"), tr("Select the threshold filter value that will be used in the motion detection algorithm. \nIncrease the value if clouds are being detected. \nRecommended value: 10"));
}

Size MainWindow::getCameraViewSize()
{
    return m_cameraViewResolution;
}

QMutex* MainWindow::cameraViewImageMutex()
{
    return &m_cameraViewImageMutex;
}

void MainWindow::setLatestStillFrame(Mat& frame)
{
    if (m_detecting && !ui->checkBoxDisplayWebcam->isChecked())
    {
        m_webcamFrame = frame.clone();
        /// @todo converting & scaling camera frame into QImage repeats many times -> refactor
        cv::Mat resizedFrame;
        cv::resize(m_webcamFrame, resizedFrame, m_cameraViewResolution, 0, 0, INTER_CUBIC);
        cv::cvtColor(resizedFrame, resizedFrame, CV_BGR2RGB);
        m_cameraViewImageMutex.lock();
        m_cameraViewImage = QImage((uchar*)resizedFrame.data, resizedFrame.cols, resizedFrame.rows, resizedFrame.step, QImage::Format_RGB888);
        m_cameraViewImageMutex.unlock();
        displayPixmap(m_cameraViewImage);   // slot
    }
}

