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
    m_polygonClosed = false;
    m_tmpLine = NULL;
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton) {
        return;
    }
    mouseEvent->accept();
    QPoint pos = mouseEvent->scenePos().toPoint();

    // moving existing node
    QGraphicsItem* clickedItem = itemAt(pos, QTransform());
    if (clickedItem && (clickedItem->type() == 4)) {    // 4 = ellipse
        clickedItem->setSelected(true);
        m_addingNode = false;
        return;
    }

    if (m_polygonClosed) {
        return;
    }

    // creating a new node
    double rad  = 4;
    QGraphicsEllipseItem* ellipse = this->addEllipse(pos.x()-rad, pos.y()-rad,
        rad*2.0, rad*2.0, QPen(Qt::red), QBrush(Qt::SolidPattern));
    ellipse->setFlag(QGraphicsItem::ItemIsSelectable, true);
    ellipse->setFlag(QGraphicsItem::ItemIsMovable, true);
    ellipse->setSelected(true);
    m_addingNode = true;

    if (!pol.isEmpty()) {
        QPointF prevNode(pol.last());
        QLineF line(prevNode, pos);
        if (m_tmpLine) {
            delete m_tmpLine;
        }
        m_tmpLine = addLine(line, QPen(Qt::red));
    }
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QPointF scenePos = event->scenePos();
    QList<QGraphicsItem*> items = selectedItems();

    if (!items.isEmpty()) {
        event->accept();
        QPointF delta = scenePos - event->lastScenePos();
        QListIterator<QGraphicsItem*> itemIt(items);

        while (itemIt.hasNext()) {
            QGraphicsItem* item = itemIt.next();
            item->moveBy(delta.x(), delta.y());
        }
    }

    if (m_addingNode && !pol.isEmpty()) {
        QPointF prevNode(pol.last());
        QLineF line(prevNode, scenePos);
        if (!m_tmpLine) {
            m_tmpLine = addLine(line, QPen(Qt::red));
        } else {
            m_tmpLine->setLine(line);
        }
    }
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    event->accept();
    QPoint pos = event->scenePos().toPoint();

    QList<QGraphicsItem*> items = selectedItems();
    if (!items.isEmpty()) {
        QListIterator<QGraphicsItem*> itemIt(items);
        while (itemIt.hasNext()) {
            QGraphicsItem* item = itemIt.next();
            item->setSelected(false);
        }
    }

    if (m_tmpLine) {
        delete m_tmpLine;
        m_tmpLine = NULL;
    }

    if (m_addingNode) {
        pol.append(pos);
        if (pol.size() > 1) {
            QPainterPath myPath;
            myPath.addPolygon(pol);
            addPath(myPath,QPen(Qt::red,1));
        }
        m_addingNode = false;
    }
}

QPolygon* GraphicsScene::detectionAreaPolygon() {
    return &pol;
}

bool GraphicsScene::connectDots()
{
    if (pol.size()>2)
    {
        addLine(QLine(pol.first(),pol.last()),QPen(Qt::red,1));
        m_polygonClosed = true;
        return true;
    }
    return false;
}

void GraphicsScene::clearPoly()
{
    pol.clear();
    m_polygonClosed = false;
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

