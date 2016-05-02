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
    this->setWindowTitle("Settings");

    this->setLayout(ui->gridLayout);

    int INDEX = m_config->cameraIndex();
    ui->comboBoxWebcam->setCurrentIndex(INDEX);

    QSize currentResolution(m_config->cameraWidth(), m_config->cameraHeight());
    QListIterator<QSize> resolutionListIt(cameraPtr->availableResolutions());
    bool currentResolutionInList = false;
    while (resolutionListIt.hasNext())
    {
        QSize resolution = resolutionListIt.next();
        QString itemStr = QString::number(resolution.width()) + " x " + QString::number(resolution.height());
        ui->comboBoxResolution->addItem(itemStr, QVariant(resolution));
        if (resolution == currentResolution) {
            currentResolutionInList = true;
            ui->comboBoxResolution->setCurrentIndex(ui->comboBoxResolution->count() - 1);
        }
    }
    if (!currentResolutionInList) {
        // show user nothing if incorrect resolution given in settings
        ui->comboBoxResolution->setCurrentIndex(ui->comboBoxResolution->count());
    }

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

    ui->comboBoxCodec->setCurrentIndex(m_config->resultVideoCodec());

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowTitleHint);
    dialogIsOpened=false;
    wasSaved=false;

//    connect(this,SIGNAL(finishedCheckingXML()),this,SLOT(cleanupThreadCheckXML()));
//    threadXMLfile.reset(new std::thread(&Settings::checkAreaFile, this));
}

void SettingsDialog::saveSettings()
{
    m_config->setResultVideoDir(ui->lineVideoPath->text());
    m_config->setCameraIndex(ui->comboBoxWebcam->currentIndex()); /// @todo cameraindex may change if cameras added/removed
    QSize selectedResolution = ui->comboBoxResolution->currentData().toSize();
    m_config->setCameraWidth(selectedResolution.width());
    m_config->setCameraHeight(selectedResolution.height());
    m_config->setDetectionAreaFile(ui->lineDetectionAreaFile->text());
    m_config->setSaveResultImages(ui->checkBoxsaveImages->isChecked());
    m_config->setResultImageDir(ui->lineImagePath->text());
    m_config->setResultVideoWithObjectRectangles(ui->checkBoxRectangle->isChecked());
    m_config->setMinPositiveDetections(ui->lineMinPosRequired->text().toInt());
    m_config->setResultVideoCodec(ui->comboBoxCodec->currentIndex());
    m_config->setUserTokenAtUfoId(ui->lineToken->text());
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
//            ui->lineStatus->setText("File found but is empty. Select area of detection");
//        }
//        else ui->lineStatus->setText("File found and loaded correctly");
//    }
	
//    emit finishedCheckingXML();
//}

void SettingsDialog::on_toolButtonVideoPath_clicked()
{
    QString videoFilePath=QFileDialog::getExistingDirectory(this,tr("Select Directory"),QDir::currentPath(),QFileDialog::ShowDirsOnly);
    ui->lineVideoPath->setText(videoFilePath);
}

SettingsDialog::~SettingsDialog()
{
    qDebug() << "destructor settings";
    //disconnect(this,SIGNAL(finishedCheckingXML()),this,SLOT(cleanupThreadCheckXML()));
    delete ui;
}


void SettingsDialog::on_buttonSave_clicked()
{
    saveSettings();
    wasSaved=true;
    QMessageBox::information(this,"Information","Restart the application to apply the changes");
}

void SettingsDialog::on_buttonCancel_clicked()
{
    if(dialogIsOpened)
    {
        myDialog->close();
        delete myDialog;
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
//    else QMessageBox::information(this,"Information","Please wait until checking of the area file has finished");
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
        tr("Select the detection area file"),QDir::currentPath(), tr("XML file (*.xml)"));
    ui->lineDetectionAreaFile->setText(detectionAreaFileName);
}

void SettingsDialog::on_toolButtonImagePath_clicked()
{
    QString imagePath = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
        QDir::currentPath(), QFileDialog::ShowDirsOnly);
    ui->lineImagePath->setText(imagePath);
}

void SettingsDialog::on_buttonSelectDetectionArea_clicked()
{
    if (wasSaved)
	{
        QMessageBox::warning(this,"Warning","Restart the application before creating detection area file");
    }
    else
	{
        dialogIsOpened=true;
        myDialog = new DetectionAreaEditDialog(0, cameraPtr, m_config);
        myDialog->setModal(true);
        myDialog->show();
        //connect(myDialog,SIGNAL(savedFile()),this,SLOT(startThreadCheckXML()));
    }
}

//void Settings::startThreadCheckXML()
//{
//    ui->lineStatus->setText("Checking...");
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

void SettingsDialog::on_toolButton_clicked()
{
    QMessageBox::information(this,"Codec Information","Raw Video results is big file sizes \nA lossless codec is recommended \nWindows 8 users should use the Lagarith Codec");
}

void SettingsDialog::on_toolButtonToken_clicked()
{
    QMessageBox::information(this,"UFOID.net User Token","Copy the User Token from your UFOID account in to this field to enable the upload feature\nThe code can be found at http://ufoid.net/profile/edit");
}
