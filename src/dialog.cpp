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

#include "dialog.h"
#include "ui_dialog.h"
#include <opencv2/highgui/highgui.hpp>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <iostream>
#include <QSettings>
#include <QDir>
#include <QtXml>

using namespace cv;
using namespace std;

/*
 * Dialog for the detection area selection
 */
Dialog::Dialog(QWidget *parent, Camera *camPtr) : QDialog(parent),  ui(new Ui::Dialog), cameraPtr(camPtr)
{
    ui->setupUi(this);

    QSettings mySettings("UFOID","Detector");
    WIDTH = mySettings.value("camerawidth",640).toInt();
    HEIGHT = mySettings.value("cameraheight",480).toInt();
    areaFilePath = mySettings.value("xmlfile",QString(QCoreApplication::applicationDirPath()+"/detectionArea.xml")).toString().toStdString();

    QFile area(areaFilePath.c_str());
    if(!area.exists())
	{
         ui->labelInfo->setText(QString("Could not find file: ") + areaFilePath.c_str() + QString(" -Check area file path in settings"));
         ui->buttonTakePicture->setEnabled(false);
         ui->buttonConnect->setEnabled(false);
         ui->buttonSave->setEnabled(false);
    }

    isPictureTaken=false;
    cout << "dialog constructed" << endl;
    ui->buttonClear->setEnabled(false);
    ui->buttonConnect->setEnabled(false);
    this->setWindowTitle("Select area of detection");
    this->setStyleSheet("background-color:#515C65; color: white");
    ui->buttonClear->setStyleSheet("background-color:#3C4A62;");
    ui->buttonConnect->setStyleSheet("background-color:#3C4A62;");
    ui->buttonSave->setStyleSheet("background-color:#3C4A62;");
    ui->buttonTakePicture->setStyleSheet("background-color:#3C4A62;");
    ui->progressBar->hide();
    this->setFixedSize(830,570);
}


/*
 * Get all points inside the selected area and pass vector with points to savePointsAsXML()
 */
void Dialog::getPointsInContour(vector<Point2f> & contour)
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
    QSettings mySettings("UFOID","Detector");
    if(insideContour.size()!=0)
	{
        mySettings.setValue("regionsize",insideContour.size());
        savePointsAsXML(insideContour);
    }
    else ui->labelInfo->setText("ERROR saving XML file. No points inside area");
}

/*
 * Save the points to area xml
 */
void Dialog::savePointsAsXML(vector<Point2f> & contour)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("UFOID");
    doc.appendChild(root);
    int count=0;
    for(int i = 0; i < contour.size(); i++)
	{
        QDomElement node = doc.createElement("point");
        node.setAttribute("x",contour[i].x);
        node.setAttribute("y",contour[i].y);
        root.appendChild(node);
        count++;
        if(count==1000)
		{
            count=0;
            float temp = (float) contour.size()/ (float) i;
            int division = 50/temp;
            ui->progressBar->setValue(division+40);
        }
    }
    ui->progressBar->setValue(90);

    QFile file(areaFilePath.c_str());
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
        QTextStream stream(&file);
        stream << doc.toString();
        ui->progressBar->setValue(100);
        file.close();
        ui->labelInfo->setText("XML file saved succesfully");

    }
    else
	{
		ui->labelInfo->setText("ERROR saving the XML file. Check \"Area file\" path in settings");
	}
        
}

void Dialog::on_buttonConnect_clicked()
{
    scene->connectDots();
    ui->labelInfo->setText("3. Press the button \"Save\" to save your XML file");
}

void Dialog::on_buttonClear_clicked()
{
    scene->clearPoly();
    scene->clear();
    ui->labelInfo->setText("1. Take a picture with the webcam");
}

void Dialog::on_buttonTakePicture_clicked()
{
    ui->progressBar->setValue(0);
    ui->progressBar->hide();
    scene = new GraphicsScene(this,cameraPtr);
    ui->image->setScene(scene);
    ui->image->show();
    isPictureTaken=true;
    ui->labelInfo->setText("2. Click to create points around the area of the dection. Connect the first and last points using the button \"Connect\"");
    ui->buttonClear->setEnabled(true);
    ui->buttonConnect->setEnabled(true);
}

void Dialog::on_buttonSave_clicked()
{
    if (isPictureTaken)
	{
        ui->progressBar->show();
        ui->progressBar->repaint();
        wasSaved=true;
        std::vector<Point2f> vecFinal = scene->getCoor();
        if (vecFinal.size()>2)
		{
            getPointsInContour(vecFinal);
            qDebug() << "saved";
        }
        else ui->labelInfo->setText("ERROR not enough points. Draw at least three points around your area of detection");
    }
    qDebug() << "saved again";
}

void Dialog::closeEvent(QCloseEvent *)
{
//    if(wasSaved)
//	  {
//        emit savedFile();
//    }
}

Dialog::~Dialog()
{
    delete ui;
}

