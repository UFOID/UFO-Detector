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

#include "detectionareaeditdialog.h"
#include "ui_detectionareaeditdialog.h"
#include <opencv2/highgui/highgui.hpp>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <iostream>
#include <QDir>
#include <QtXml>

using namespace cv;
using namespace std;

DetectionAreaEditDialog::DetectionAreaEditDialog(QWidget *parent, Camera *camPtr, Config *configPtr) :
    QDialog(parent),  ui(new Ui::DetectionAreaEditDialog), cameraPtr(camPtr), m_config(configPtr)
{
    ui->setupUi(this);

    WIDTH = m_config->cameraWidth();
    HEIGHT = m_config->cameraHeight();
    areaFilePath = m_config->detectionAreaFile().toStdString(); /// @todo no need to use stdstrings

    QFile area(areaFilePath.c_str());
    if(!area.exists())
	{
         ui->labelInfo->setText(tr("Could not find area file: %1 - Check area file path in settings").arg(areaFilePath.c_str()));
         ui->buttonTakePicture->setEnabled(false);
         ui->buttonConnect->setEnabled(false);
         ui->buttonSave->setEnabled(false);
    }

    isPictureTaken=false;
    cout << "dialog constructed" << endl;
    ui->buttonClear->setEnabled(false);
    ui->buttonConnect->setEnabled(false);
    ui->progressBar->hide();
}

void DetectionAreaEditDialog::getPointsInContour(vector<Point2f> & contour)
{
    vector<Point2f> insideContour;
    for(int j = 0; j < HEIGHT; j++)
	{
        for(int i = 0; i < WIDTH; i++)
		{
            Point2f p(i,j);
            if(pointPolygonTest(contour,p,false) >= 0)
			{ // yes inside
				insideContour.push_back(p);
			}                
        }
        float temp = (float)HEIGHT/(float)(j+1);
        int division = 40/temp;
        ui->progressBar->setValue(division);
    }
    cout << "# points inside contour: " << insideContour.size() << endl;
    if(insideContour.size()!=0)
    {
        m_config->setDetectionAreaSize((int)insideContour.size());
        savePointsAsXML(insideContour);
    }
    else ui->labelInfo->setText(tr("ERROR saving detection area file: no points inside area"));
}

void DetectionAreaEditDialog::savePointsAsXML(vector<Point2f> & contour)
{
    QFile file(QApplication::applicationDirPath() + "/detectionarea-point.xml");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
       QXmlStreamWriter stream(&file);
       stream.setAutoFormatting(true);
       stream.writeStartDocument();
       stream.writeStartElement("UFOID");

       int count=0;
       for(int i = 0; i < (int)contour.size(); i++)
       {
           count++;

           stream.writeStartElement("point");
           stream.writeAttribute("x",QString::number(contour[i].x));
           stream.writeAttribute("y",QString::number(contour[i].y));
           stream.writeEndElement();
           if(count==1000)
           {
               count=0;
               float temp = (float) contour.size()/ (float) i;
               int division = 50/temp;
               ui->progressBar->setValue(division+40);
           }
       }
       stream.writeEndElement();
       ui->progressBar->setValue(100);


       file.close();
       ui->labelInfo->setText(tr("Detection area file saved successfully"));

    }
    else
    {
        ui->labelInfo->setText(tr("ERROR saving the detection area file. Check \"Detection area file\" path in settings."));
    }
}

bool DetectionAreaEditDialog::savePolygonsAsXml() {
    QFile file(QApplication::applicationDirPath() + "/detectionarea-polygon.xml");
    QPolygon* polygon = scene->detectionAreaPolygon();

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ui->labelInfo->setText("ERROR saving " + file.fileName());
        return false;
    }
    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("detectionareas");
    stream.writeStartElement("detectionarea");
    stream.writeStartElement("camera");
    stream.writeAttribute("id", QString::number(m_config->cameraIndex()));
    stream.writeAttribute("width", QString::number(m_config->cameraWidth()));
    stream.writeAttribute("height", QString::number(m_config->cameraHeight()));
    stream.writeEndElement(); // camera

    stream.writeStartElement("polygon");
    for(int i = 0; i < polygon->size(); i++) {
        stream.writeStartElement("point");
        stream.writeAttribute("x", QString::number(polygon->at(i).x()));
        stream.writeAttribute("y", QString::number(polygon->at(i).y()));
        stream.writeEndElement();
    }
    stream.writeEndElement(); // polygon
    stream.writeEndElement(); // detectionarea
    stream.writeEndElement(); // detectionareas
    stream.writeEndDocument();
    ui->progressBar->setValue(100);

    file.close();
    ui->labelInfo->setText("Saved " + file.fileName());
    return true;
}

void DetectionAreaEditDialog::on_buttonConnect_clicked()
{
    scene->connectDots();
    ui->labelInfo->setText(tr("3. Press the \"Save\" button to save your detection area file"));
}

void DetectionAreaEditDialog::on_buttonClear_clicked()
{
    scene->clearPoly();
    scene->clear();
    ui->labelInfo->setText(tr("1. Take a picture with the webcam"));
}

void DetectionAreaEditDialog::on_buttonTakePicture_clicked()
{
    ui->progressBar->setValue(0);
    ui->progressBar->hide();
    scene = new GraphicsScene(this,cameraPtr);
    ui->image->setScene(scene);
    ui->image->show();
    isPictureTaken=true;
    ui->labelInfo->setText(tr("2. Click to create points around the area of the dection. Connect the first and last points using the \"Connect\" button."));
    ui->buttonClear->setEnabled(true);
    ui->buttonConnect->setEnabled(true);
}

void DetectionAreaEditDialog::on_buttonSave_clicked()
{
    chrono::high_resolution_clock::time_point startTime, endTime;
    startTime = chrono::high_resolution_clock::now();
    if (isPictureTaken)
    {
        ui->progressBar->show();
        ui->progressBar->repaint();
        wasSaved=true;
        std::vector<Point2f> vecFinal = scene->getCoor();
        if (vecFinal.size()>2)
        {
            getPointsInContour(vecFinal);
        }
        else ui->labelInfo->setText(tr("ERROR not enough points. Draw at least three points around your area of detection."));
    }
    endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> msec = endTime - startTime;
    qDebug() << "Saving detection area point file took" << msec.count() << "ms";
}

void DetectionAreaEditDialog::on_savePolygonFilePushButton_clicked() {
    chrono::high_resolution_clock::time_point startTime, endTime;
    startTime = chrono::high_resolution_clock::now();
    if (isPictureTaken)
    {
        ui->progressBar->show();
        ui->progressBar->repaint();
        if (scene->detectionAreaPolygon()->size() > 2) {
            savePolygonsAsXml();
            wasSaved=true;
        }
        else ui->labelInfo->setText(tr("ERROR not enough points. Draw at least three points around your area of detection."));
    }
    endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> msec = endTime - startTime;
    qDebug() << "Saving detection area polygon file took" << msec.count() << "ms";
}

void DetectionAreaEditDialog::closeEvent(QCloseEvent *)
{
//    if(wasSaved)
//	  {
//        emit savedFile();
//    }
}

DetectionAreaEditDialog::~DetectionAreaEditDialog()
{
    delete ui;
}

