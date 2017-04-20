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

DetectionAreaEditDialog::DetectionAreaEditDialog(QWidget *parent, Camera *camPtr, Config *configPtr) :
    QDialog(parent),  ui(new Ui::DetectionAreaEditDialog), m_camera(camPtr), m_config(configPtr)
{
    ui->setupUi(this);

    QFile area(m_config->detectionAreaFile());
    if(!area.exists())
    {
         ui->labelInfo->setText(tr("Could not find area file: %1 - Check area file path in settings").arg(m_config->detectionAreaFile()));
         ui->buttonTakePicture->setEnabled(false);
    }

    m_scene = new GraphicsScene(this, m_camera);
    ui->image->setScene(m_scene);
    ui->image->show();
    //ui->image->setMouseTracking(true);
    m_pictureTaken = false;
    ui->buttonClear->setEnabled(false);
    ui->buttonConnect->setEnabled(false);
    ui->buttonSave->setEnabled(false);
}

bool DetectionAreaEditDialog::savePolygonsAsXml() {
    QFile file(m_config->detectionAreaFile());
    QPolygon* polygon = m_scene->detectionAreaPolygon();

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ui->labelInfo->setText(tr("ERROR saving %1").arg(file.fileName()));
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

    file.close();
    ui->labelInfo->setText(tr("Saved %1").arg(file.fileName()));

    return true;
}

bool DetectionAreaEditDialog::checkPolygon(QPolygon* polygon) {
    if (!polygon) {
        return false;
    }

    if (polygon->size() <= 2) {
        return false;
    }

    QRect boundingRect = polygon->boundingRect();

    for (int bx = boundingRect.x(); bx < boundingRect.width(); bx++) {
        for (int by = boundingRect.y(); by < boundingRect.height(); by++) {
            if (polygon->containsPoint(QPoint(bx, by), Qt::OddEvenFill)) {
                return true;
            }
        }
    }
    return false;
}

void DetectionAreaEditDialog::on_buttonConnect_clicked()
{
    if (m_scene->connectDots()) {
        ui->buttonSave->setEnabled(true);
        ui->labelInfo->setText(tr("3. Press the \"Save\" button to save your detection area file"));
    } else {
        ui->labelInfo->setText(tr("ERROR not enough points. Draw at least three points around your area of detection."));
    }
}

void DetectionAreaEditDialog::on_buttonClear_clicked()
{
    m_scene->clearPoly();
    m_scene->clear();
    ui->buttonSave->setEnabled(false);
    ui->labelInfo->setText(tr("1. Take a picture with the webcam"));
}

void DetectionAreaEditDialog::on_buttonTakePicture_clicked()
{
    m_scene->clearPoly();
    m_scene->clear();
    m_scene->takePicture();
    m_pictureTaken = true;
    ui->labelInfo->setText(tr("2. Click to create points around the area of the detection. Connect the first and last points using the \"Connect\" button."));
    ui->buttonClear->setEnabled(true);
    ui->buttonConnect->setEnabled(true);
}

void DetectionAreaEditDialog::on_buttonSave_clicked() {
    if (m_pictureTaken)
    {
        QPolygon* polygon = m_scene->detectionAreaPolygon();
        if (checkPolygon(polygon)) {
            savePolygonsAsXml();
        }
    }
}

DetectionAreaEditDialog::~DetectionAreaEditDialog()
{
    delete ui;
}

