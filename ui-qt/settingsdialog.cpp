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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <QMessageBox>
#include <QDebug>
#include <QDomDocument>


/* Note:
 * Checking the status of the area file is broken. Commented out
 */

SettingsDialog::SettingsDialog(QWidget *parent, Camera *camPtr, Config *configPtr) :
    QDialog(parent), ui(new Ui::SettingsDialog), cameraPtr(camPtr), m_config(configPtr)
{
    ui->setupUi(this);

    this->setLayout(ui->gridLayout);

    int INDEX = m_config->cameraIndex();
    ui->comboBoxWebcam->setCurrentIndex(INDEX);
    ui->lineEditCameraWidth->setText(QString::number(m_config->cameraWidth()));
    ui->lineEditCameraHeight->setText(QString::number(m_config->cameraHeight()));
    ui->lineEditCameraWidth->setValidator(new QIntValidator());
    ui->lineEditCameraHeight->setValidator(new QIntValidator());
    ui->lineVideoPath->setText(m_config->resultVideoDir());
    ui->lineDetectionAreaFile->setText(m_config->detectionAreaFile());
    ui->lineToken->setText(m_config->userTokenAtUfoId());
    if(m_config->saveResultImages())
	{
        ui->checkBoxsaveImages->setChecked(true);
        emit on_checkBoxsaveImages_stateChanged(Qt::Checked);
    }
    else emit on_checkBoxsaveImages_stateChanged(Qt::Unchecked);
	
    ui->checkBoxRectangle->setChecked(m_config->resultVideoWithObjectRectangles());
    ui->lineImagePath->setText(m_config->resultImageDir());
    ui->lineMinPosRequired->setText(QString::number(m_config->minPositiveDetections()));

    VideoCodecSupportInfo* codecInfo = m_config->videoCodecSupportInfo();
    QListIterator<int> supportedCodecsIt(codecInfo->supportedCodecs());
    while (supportedCodecsIt.hasNext()) {
        int codec = supportedCodecsIt.next();
        ui->comboBoxCodec->addItem(codecInfo->codecName(codec), QVariant(codecInfo->fourccToString(codec)));
    }
    ui->comboBoxCodec->setCurrentIndex(ui->comboBoxCodec->findData(
        QVariant(m_config->resultVideoCodecStr())));
    if (ui->comboBoxCodec->currentIndex() < 0) {
        ui->comboBoxCodec->setCurrentIndex(0);
    }
    if (m_config->checkAirplanes()){
        ui->checkBoxFilterAiplanes->setChecked(Qt::Checked);
        ui->lineEditFilterAirplanes->setText(m_config->coordinates());
        emit on_checkBoxFilterAiplanes_clicked();
    }
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowTitleHint);
    m_detectionAreaDialogIsOpen=false;
    wasSaved=false;

//    connect(this,SIGNAL(finishedCheckingXML()),this,SLOT(cleanupThreadCheckXML()));
//    threadXMLfile.reset(new std::thread(&Settings::checkAreaFile, this));
}

void SettingsDialog::saveSettings()
{
    m_config->setResultVideoDir(ui->lineVideoPath->text());
    m_config->setCameraIndex(ui->comboBoxWebcam->currentIndex()); /// @todo cameraindex may change if cameras added/removed
    m_config->setCameraWidth(ui->lineEditCameraWidth->text().toInt());
    m_config->setCameraHeight(ui->lineEditCameraHeight->text().toInt());
    m_config->setDetectionAreaFile(ui->lineDetectionAreaFile->text());
    m_config->setSaveResultImages(ui->checkBoxsaveImages->isChecked());
    m_config->setResultImageDir(ui->lineImagePath->text());
    m_config->setResultVideoWithObjectRectangles(ui->checkBoxRectangle->isChecked());
    m_config->setMinPositiveDetections(ui->lineMinPosRequired->text().toInt());
    m_config->setResultVideoCodec(ui->comboBoxCodec->currentData().toString());
    m_config->setUserTokenAtUfoId(ui->lineToken->text());
    m_config->setCheckAirplanes(ui->checkBoxFilterAiplanes->isChecked());  

    if (ui->checkBoxFilterAiplanes->isChecked()){
        QString value = ui->lineEditFilterAirplanes->text();
        if (value.isEmpty() || value.split("\n").size() != 3){
            m_config->setCheckAirplanes(false);
            QMessageBox::critical(this, tr("Error"), tr("Value entered for Airplane filter not correct. Click the \"?\" button next to the field for more information."));
        }
        else m_config->setAirplanesCoordinates(ui->lineEditFilterAirplanes->text());
    }
}

//void Settings::checkAreaFile()
//{
//    QFile fileXML(xmlFile);
//    QDomDocument doc;

//    if(!fileXML.open(QIODevice::ReadOnly | QIODevice::Text))
//	{
//        qDebug() << "fail reading the area file";
//    }
//    else
//	{
//        if(!doc.setContent(&fileXML))
//		{
//            ui->lineStatus->setText(tr("File found but is empty. Select area of detection."));
//        }
//        else ui->lineStatus->setText(tr("File found and loaded correctly"));
//    }
	
//    emit finishedCheckingXML();
//}

void SettingsDialog::on_toolButtonVideoPath_clicked()
{
    QString videoFilePath=QFileDialog::getExistingDirectory(this,tr("Select Directory"),QDir::currentPath(),QFileDialog::ShowDirsOnly);
    if (!videoFilePath.isEmpty())
    {
        ui->lineVideoPath->setText(videoFilePath);
    }
}

SettingsDialog::~SettingsDialog()
{
    qDebug() << "destructor settings";
    //disconnect(this,SIGNAL(finishedCheckingXML()),this,SLOT(cleanupThreadCheckXML()));
    delete ui;
}


void SettingsDialog::on_buttonSave_clicked()
{
    // check camera frame aspect ratio
    int cameraWidth = ui->lineEditCameraWidth->text().toInt();
    int cameraHeight = ui->lineEditCameraHeight->text().toInt();
    int cameraAspectRatio =  (double)cameraWidth / (double)cameraHeight * 10000;
    if (!cameraPtr->knownAspectRatios().contains(cameraAspectRatio))
    {
        QMessageBox::warning(this, tr("Error"), tr("Unknown camera aspect ratio. Please change camera width and/or height. Settings are not saved."));
        return;
    }
    saveSettings();
    wasSaved=true;
    /// @todo apply settings on-the-fly
    QMessageBox::information(this, tr("Information"), tr("Settings saved successfully. Restart the application to apply the changes."));
}

void SettingsDialog::on_buttonCancel_clicked()
{
    if(m_detectionAreaDialogIsOpen)
    {
        m_detectionAreaDialog->close();
        delete m_detectionAreaDialog;
    }
    this->close();
//    if(!threadXMLfile)
//	{
//        if(dialogIsOpened)
//		{
//            disconnect(myDialog,SIGNAL(savedFile()),this,SLOT(startThreadCheckXML()));
//            myDialog->close();
//            delete myDialog;
//        }
//        this->close();
//    }
//    else QMessageBox::information(this, tr("Information"), tr("Please wait until checking of the area file has finished"));
}

void SettingsDialog::on_checkBoxsaveImages_stateChanged(int arg1)
{
    if(arg1==Qt::Checked)
	{
        ui->labelImagePath->setEnabled(true);
        ui->lineImagePath->setEnabled(true);
        ui->toolButtonImagePath->setEnabled(true);
    }
    if(arg1==Qt::Unchecked)
	{
        ui->labelImagePath->setDisabled(true);
        ui->lineImagePath->setDisabled(true);
        ui->toolButtonImagePath->setDisabled(true);
    }
}

void SettingsDialog::on_toolButtonDetectionAreaFile_clicked()
{
    /// @todo use previous detection area file name if user cancels file selection
    QString detectionAreaFileName = QFileDialog::getOpenFileName(this,
        tr("Select the detection area file"),QDir::currentPath(), tr("XML files (*.xml)"));
    if (!detectionAreaFileName.isEmpty())
    {
        ui->lineDetectionAreaFile->setText(detectionAreaFileName);
    }
}

void SettingsDialog::on_toolButtonImagePath_clicked()
{
    QString imagePath = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        QDir::currentPath(), QFileDialog::ShowDirsOnly);
    if (!imagePath.isEmpty())
    {
        ui->lineImagePath->setText(imagePath);
    }
}

void SettingsDialog::on_buttonSelectDetectionArea_clicked()
{
    if (wasSaved)
	{
        QMessageBox::warning(this, tr("Warning"), tr("Restart the application before creating detection area file"));
    }
    else
	{
        m_detectionAreaDialogIsOpen=true;
        m_detectionAreaDialog = new DetectionAreaEditDialog(0, cameraPtr, m_config);
        m_detectionAreaDialog->setModal(true);
        m_detectionAreaDialog->show();
        //connect(myDialog,SIGNAL(savedFile()),this,SLOT(startThreadCheckXML()));
    }
}

void SettingsDialog::on_toolButtonResolutionDialog_clicked()
{
    m_resolutionDialog = new CameraResolutionDialog(cameraPtr, m_config, this);
    connect(m_resolutionDialog, SIGNAL(resolutionAccepted(QSize)), this, SLOT(onResolutionAcceptedInDialog(QSize)));
    m_resolutionDialog->show();
}

void SettingsDialog::onResolutionAcceptedInDialog(QSize resolution)
{
    ui->lineEditCameraWidth->setText(QString::number(resolution.width()));
    ui->lineEditCameraHeight->setText(QString::number(resolution.height()));
}

//void Settings::startThreadCheckXML()
//{
//    ui->lineStatus->setText(tr("Checking..."));
//    threadXMLfile.reset(new std::thread(&Settings::checkAreaFile, this));
//}

//void Settings::cleanupThreadCheckXML()
//{
//    if(threadXMLfile)
//	{
//        threadXMLfile->join();
//        threadXMLfile.reset();
//        qDebug() << "cleanup done";
//    }
//}

void SettingsDialog::on_toolButtonCodecHelp_clicked()
{
    QMessageBox::information(this, tr("Codec Information"), tr("Raw Video results is big file sizes. \nA lossless codec is recommended. \nWindows 8 users should use the Lagarith Codec."));
}

void SettingsDialog::on_toolButtonTokenHelp_clicked()
{
    QMessageBox::information(this, tr("UFOID.net User Token"), tr("Copy the user token from your UFOID account into this field to enable the upload feature. \nThe code can be found at http://ufoid.net/profile/edit"));
}

void SettingsDialog::on_checkBoxFilterAiplanes_clicked()
{
    ui->lineEditFilterAirplanes->setEnabled(ui->checkBoxFilterAiplanes->isChecked());
}

void SettingsDialog::on_toolButtonFilterAirplanes_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Plane Filter Information");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText("To use the airplane filter enter the location coordinates in to the text field.<br/>Visit <a href=http://ufoid.net/airplanes.html>http://ufoid.net/airplanes.html</a> to specify your location. \nThe airplane filter requires an active internet connection.");
    msgBox.exec();
}
