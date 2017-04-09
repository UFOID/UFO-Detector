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

MainWindow::MainWindow(Camera* cameraPtr, Config* configPtr, DataManager* dataManager, QWidget* parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "Constructing MainWindow";

    QWidget* mainWindowCentralWidget = new QWidget(this);
    mainWindowCentralWidget->setLayout(ui->mainWindowLayout);
    setCentralWidget(mainWindowCentralWidget);

    m_camera = cameraPtr;
    m_config = configPtr;
    m_dataManager = dataManager;

    m_programVersion = APPLICATION_VERSION;

    this->setWindowTitle("UFO Detector | BETA " + m_programVersion);

    const int width = m_config->cameraWidth();
    const int height = m_config->cameraHeight();
    m_cameraViewResolution = Size(width, height);
    const int NOISELEVEL = m_config->noiseFilterPixelSize();
    QString VERSION = m_config->applicationVersion();

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

#if defined(Q_OS_WINDOWS)
    VideoCodecSupportInfo* codecInfo = m_config->videoCodecSupportInfo();
    codecInfo->removeSupport(CV_FOURCC('F', 'F', 'V', '1'), VideoCodecSupportInfo::OpenCv);
#endif

    m_dataManager->init();

    ui->statusLabel->setStyleSheet(m_detectionStatusStyleOff);
    ui->recordingTestButton->hide();
    ui->progressBar->hide();
    ui->outputText->append(tr("For feedback and bug reports contact the developer at contact@ufoid.net"));

    if (checkCamera(width, height))
    {
        threadWebcam.reset(new std::thread(&MainWindow::updateWebcamFrame, this));
    }

    //Add VideoWidget to UI
    QDomNode node = m_dataManager->resultDataDomDocument()->firstChildElement().firstChild();
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
    ui->videoList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->videoList->scrollToBottom();

    //Check for new version
    if (m_config->checkApplicationUpdates()) {
        connect(m_dataManager, SIGNAL(newApplicationVersionAvailable(QString,std::queue<QString>)),
                this, SLOT(onNewApplicationVersionAvailable(QString,std::queue<QString>)));
        m_dataManager->checkForUpdates();
    }
    qDebug() << "MainWindow constructed" ;
}

/*
 * Read webcamframe from webcam
 */
void MainWindow::updateWebcamFrame()
{
    cv::Mat frame;
    int availableFrameTime = 1000/24;   // ms per frame
    qint64 frameStartTime;
    qint64 frameEndTime;
    int frameTimeLeft;
    while (m_showCameraVideo)
    {
        frameStartTime = QDateTime::currentMSecsSinceEpoch();

        m_webcamFrame = m_camera->getWebcamFrame();
        cv::cvtColor(m_webcamFrame, frame, CV_BGR2RGB);
        m_cameraViewImage = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        emit updatePixmap(m_cameraViewImage);

        frameEndTime = QDateTime::currentMSecsSinceEpoch();
        frameTimeLeft = availableFrameTime - (frameEndTime - frameStartTime);
        // Sleep to keep frame rate of camera view ~constant,
        // but more importantly if not sleeping then emitting signals too fast
        if (frameTimeLeft > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(frameTimeLeft));
        }
        std::this_thread::yield();
    }
}

void MainWindow::displayPixmap(QImage image)
{
    m_cameraViewImageMutex.lock();
    m_latestCameraViewVideoFrame = image.copy();
    QImage scaledImage = image.scaled(m_cameraViewResolution.width, m_cameraViewResolution.height,
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->cameraView->setPixmap(QPixmap::fromImage(scaledImage));
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
    connect(m_dataManager, SIGNAL(resultDataSaved(QString,QString,QString)), this, SLOT(addVideoToList(QString,QString,QString)));
    connect(m_actualDetector->getRecorder(), SIGNAL(recordingStarted()), this, SLOT(onRecordingStarted()));
    connect(m_actualDetector->getRecorder(), SIGNAL(recordingFinished()), this, SLOT(onRecordingFinished()));
    connect(this, SIGNAL(elementWasRemoved()), m_dataManager, SLOT(readResultDataFile()));
    connect(m_actualDetector, SIGNAL(progressValueChanged(int)), this, SLOT(on_progressBar_valueChanged(int)));
    connect(m_actualDetector, SIGNAL(broadcastOutputText(QString)), this, SLOT(update_output_text(QString)));
    connect(this, SIGNAL(updatePixmap(QImage)), this, SLOT(displayPixmap(QImage)));
    connect(ui->videoList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onVideoListContextMenuRequested(const QPoint&)));

    if (m_config->checkAirplanes()){
        m_planeChecker = new PlaneChecker(this,m_config->coordinates());
        connect(m_planeChecker, SIGNAL(foundNumberOfPlanes(int)), m_actualDetector, SLOT(setAmountOfPlanes(int)));
        connect(m_actualDetector, SIGNAL(checkPlane()), m_planeChecker, SLOT(callApi()));
    }
}

/*
 * Add new VideoWidget element to video list
 */
void MainWindow::addVideoToList(QString filename, QString dateTime, QString videoLength)
{
    bool scroll = false;
    VideoWidget* newWidget = new VideoWidget(this, filename, dateTime, videoLength);
    connect(newWidget->deleteButton(), SIGNAL(clicked()), this, SLOT(onVideoDeleteClicked()));
    connect(newWidget->uploadButton(), SIGNAL(clicked()), this, SLOT(onVideoUploadClicked()));
    connect(newWidget->playButton(), SIGNAL(clicked()), this, SLOT(onVideoPlayClicked()));
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(150,100));

    QScrollBar* scrollBar = ui->videoList->verticalScrollBar();
    if (scrollBar && (scrollBar->value() == scrollBar->maximum())) {
        scroll = true;
    }

    ui->videoList->addItem(item);
    ui->videoList->setItemWidget(item,newWidget);

    if (scroll) {
        ui->videoList->scrollToBottom();
    }

    recordingCounter_++;
    ui->lineCount->setText(QString::number(recordingCounter_));
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
            VideoUploaderDialog* upload = new VideoUploaderDialog(this, widget->videoFileName(), m_config);
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

void MainWindow::removeVideo(QString dateTime) {
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
            m_dataManager->removeVideo(dateTime);
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

        m_actualDetector->setNoiseLevel(ui->sliderNoise->value());
        m_actualDetector->setThresholdLevel(ui->sliderThresh->value());

        if(m_actualDetector->start())
        {
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

void MainWindow::on_checkBoxDisplayWebcam_stateChanged(int arg1)
{
    if (Qt::Unchecked == arg1)
    {
        m_actualDetector->setShowCameraVideo(false);
    }
    else if (Qt::Checked == arg1)
    {
        m_actualDetector->setShowCameraVideo(true);
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
    // if image is not shown anywhere else do it here
    if (m_detecting && !ui->checkBoxDisplayWebcam->isChecked())
    {
        displayPixmap(m_latestCameraViewVideoFrame);   // slot
    }
    emit cameraViewFrameSizeChanged(cameraFrameSize);
}

bool MainWindow::checkCamera(const int width, const int height)
{
    bool success = false;
    if (checkAndSetResolution(width, height) && !threadWebcam && m_camera->isWebcamOpen())
    {
        try
        {
            m_webcamFrame = m_camera->getWebcamFrame();
            cv::resize(m_webcamFrame, m_webcamFrame, m_cameraViewResolution, 0, 0, INTER_CUBIC);
            success = true;
        }
        catch( cv::Exception& e )
        {
            qDebug() << "Exception caught: " << QString(e.what());
            ui->outputText->append(tr("ERROR: Found webcam but video frame could not be read. Reconnect and check resolution in settings."));
        }
    }
    else if (!m_camera->isWebcamOpen())
    {
        ui->outputText->append("ERROR: Could not open webcam. Select webcam in settings");
    }

    ui->startButton->setEnabled(success);

    return success;
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

void MainWindow::onNewApplicationVersionAvailable(QString newVersion, std::queue<QString> messageInXml) {
    m_updateApplicationDialog = new UpdateApplicationDialog(this, newVersion, messageInXml);
    m_updateApplicationDialog->show();
    m_updateApplicationDialog->setAttribute(Qt::WA_DeleteOnClose);
}

