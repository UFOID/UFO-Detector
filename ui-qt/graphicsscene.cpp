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

#include "graphicsscene.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <QPainter>

using namespace cv;
using namespace std;

/*
 * Graphics scene for drawing points to the detection area dialog
 */
GraphicsScene::GraphicsScene(QObject *parent, Camera *camera) :
    QGraphicsScene(parent)
{
    m_camera = camera;
    m_picture = NULL;
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
	{
        QPoint pos = mouseEvent->scenePos().toPoint();
        pol.append(pos);
        double rad  = 2;
        this->addEllipse(pos.x()-rad, pos.y()-rad, rad*2.0, rad*2.0, QPen(Qt::red), QBrush(Qt::SolidPattern));
        if(pol.size() > 1)
		{
            QPainterPath myPath;
            myPath.addPolygon(pol);
            addPath(myPath,QPen(Qt::red,1));
        }
    }
}

QPolygon* GraphicsScene::detectionAreaPolygon() {
    return &pol;
}

void GraphicsScene::connectDots()
{
    if (pol.size()>2)
	{
       addLine(QLine(pol.first(),pol.last()),QPen(Qt::red,1));
    }

}

void GraphicsScene::clearPoly()
{
    pol.clear();
}

bool GraphicsScene::takePicture() {
    Mat src;
    src = m_camera->getWebcamFrame();
    cv::cvtColor(src, src, CV_BGR2RGB);
    QImage imgToDisplay = QImage((uchar*)src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
    if (items().contains((QGraphicsItem*)m_picture)) {
        removeItem((QGraphicsItem*)m_picture);
    }
    m_picture = addPixmap(QPixmap::fromImage(imgToDisplay));
    return true;
}

GraphicsScene::~GraphicsScene()
{
    qDebug() << "deleted scene";
}

