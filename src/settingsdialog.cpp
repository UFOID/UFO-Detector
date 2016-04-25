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
#include <QSettings>
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

    QSettings settings("UFOID","Detector");
    int INDEX=settings.value("cameraindex",0).toInt();
    ui->comboBoxWebcam->setCurrentIndex(INDEX);
    ui->lineW->setText(settings.value("camerawidth",640).toString());
    ui->lineH->setText(settings.value("cameraheight",480).toString());
    ui->lineW->setValidator(new QIntValidator(10, 1280, this));
    ui->lineH->setValidator(new QIntValidator(10, 768, this));
    ui->lineVideoPath->setText(settings.value("videofilepath").toString());
    ui->lineDetectionAreaFile->setText(m_config->detectionAreaFile());
    ui->lineToken->setText(settings.value("usertoken","").toString());
    if(settings.value("saveimages").toBool())
	{
        ui->checkBoxsaveImages->setChecked(true);
        emit on_checkBoxsaveImages_stateChanged(Qt::Checked);
    }
    else emit on_checkBoxsaveImages_stateChanged(Qt::Unchecked);
	
    ui->checkBoxRectangle->setChecked(settings.value("recordwithrect",false).toBool());
    ui->lineImagePath->setText(settings.value("imagespath").toString());
    ui->lineMinPosRequired->setText(settings.value("minpositive",2).toString());

    if (QSysInfo::windowsVersion()==QSysInfo::WV_WINDOWS8 || QSysInfo::windowsVersion()==QSysInfo::WV_WINDOWS8_1)
	{
        ui->comboBoxCodec->setCurrentIndex(settings.value("videocodec",2).toInt());
    }
    else ui->comboBoxCodec->setCurrentIndex(settings.value("videocodec",1).toInt());

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowTitleHint);
    dialogIsOpened=false;
    wasSaved=false;

//    connect(this,SIGNAL(finishedCheckingXML()),this,SLOT(cleanupThreadCheckXML()));
//    threadXMLfile.reset(new std::thread(&Settings::checkAreaFile, this));
}

void SettingsDialog::saveSettings()
{
    QSettings settings("UFOID","Detector");
    settings.setValue("videofilepath",ui->lineVideoPath->text());
    settings.setValue("cameraindex",ui->comboBoxWebcam->currentIndex()); /// @todo cameraindex may change if cameras added/removed
    settings.setValue("camerawidth",ui->lineW->text());
    settings.setValue("cameraheight",ui->lineH->text());
    m_config->setDetectionAreaFile(ui->lineDetectionAreaFile->text());
    settings.setValue("saveimages",ui->checkBoxsaveImages->isChecked());
    settings.setValue("imagespath",ui->lineImagePath->text());
    settings.setValue("recordwithrect",ui->checkBoxRectangle->isChecked());
    settings.setValue("minpositive",ui->lineMinPosRequired->text());
    settings.setValue("videocodec",ui->comboBoxCodec->currentIndex());
    settings.setValue("usertoken",ui->lineToken->text());
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
