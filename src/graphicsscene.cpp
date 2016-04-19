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

#include "graphicsscene.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "QSettings"
#include <QPainter>
#include "camera.h"

using namespace cv;
using namespace std;

/*
 * Graphics scene for drawing points to the detection area dialog
 */
GraphicsScene::GraphicsScene(QObject *parent, Camera *camPtr) :
    QGraphicsScene(parent)
{
    Mat src;    
    src = camPtr->getWebcamFrame();
    cv::cvtColor(src, src, CV_BGR2RGB);
    QImage imgToDisplay = QImage((uchar*)src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
    addPixmap(QPixmap::fromImage(imgToDisplay));
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

std::vector<Point2f> GraphicsScene::getCoor()
{
    std::vector<Point2f> vecPoint;

    if (pol.size()>2)
	{
        connectDots();
        std::vector<QPoint> myVec = pol.toStdVector();

        for(int i=0; i<myVec.size(); i++)
		{
            QPoint point = myVec[i];
            Point2f p(point.x(), point.y());
           vecPoint.push_back(p);
        }

        pol.clear();
        return vecPoint;
    }
    else
	{
         cout << "empty vector" << endl;
         return vecPoint;
    }

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

GraphicsScene::~GraphicsScene()
{
    qDebug() << "deleted scene";
}

