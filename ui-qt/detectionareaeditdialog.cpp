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
    ui->buttonClear->setEnabled(false);
    ui->buttonConnect->setEnabled(false);
    ui->progressBar->hide();
}

bool DetectionAreaEditDialog::savePolygonsAsXml() {
    QFile file(m_config->detectionAreaFile());
    QPolygon* polygon = scene->detectionAreaPolygon();

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ui->labelInfo->setText("ERROR saving " + file.fileName());
        return false;
    }

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);

    stream.writeStartDocument();
    stream.writeStartElement("detectionarealist");
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
    stream.writeEndElement(); // detectionarealist
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

void DetectionAreaEditDialog::on_buttonSave_clicked() {
    if (isPictureTaken)
    {
        ui->progressBar->show();
        ui->progressBar->repaint();
        if (scene->detectionAreaPolygon()->size() > 2) {
            /// @todo check area of polygon >= 1
            savePolygonsAsXml();
        }
        else ui->labelInfo->setText(tr("ERROR not enough points. Draw at least three points around your area of detection."));
    }
}

DetectionAreaEditDialog::~DetectionAreaEditDialog()
{
    delete ui;
}

