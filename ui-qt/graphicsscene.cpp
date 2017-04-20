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
    if (clickedItem && (clickedItem->type() == PolygonNode::Type)) {
        clickedItem->grabMouse();
        m_addingNode = false;
        return;
    }

    if (m_polygonClosed) {
        return;
    }

    // creating a new node
    m_tmpNode = new PolygonNode();
    this->addItem(m_tmpNode);
    m_tmpNode->setPos(pos);
    m_tmpNode->setZValue(10);
    m_tmpNode->grabMouse();
    m_addingNode = true;

    if (!m_polygonNodes.isEmpty()) {
        QPointF prevPos = m_polygonNodes.last()->pos();
        QLineF line(prevPos, pos);
        if (m_tmpLine) {
            delete m_tmpLine;
        }
        m_tmpLine = addLine(line, QPen(Qt::red));
    }
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QPointF scenePos = event->scenePos();
    QGraphicsItem* draggedItem = this->mouseGrabberItem();

    event->accept();

    if (draggedItem) {
        QPointF delta = scenePos - event->lastScenePos();
        draggedItem->moveBy(delta.x(), delta.y());
    }

    if (m_addingNode && !m_polygonNodes.isEmpty()) {
        QPointF prevPos = m_polygonNodes.last()->pos();
        QLineF line(prevPos, scenePos);
        if (!m_tmpLine) {
            m_tmpLine = addLine(line, QPen(Qt::red));
        } else {
            m_tmpLine->setLine(line);
        }
    }
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    event->accept();

    QGraphicsItem* draggedItem = this->mouseGrabberItem();
    if (draggedItem) {
        draggedItem->ungrabMouse();
    }

    if (m_tmpLine) {
        delete m_tmpLine;
        m_tmpLine = NULL;
    }

    if (m_addingNode) {
        if (m_polygonNodes.size() > 0) {
            PolygonNode* prevNode = m_polygonNodes.last();
            m_polygonNodes.append(m_tmpNode);
            PolygonEdge* edge = new PolygonEdge(prevNode, m_tmpNode);
            this->addItem(edge);
            m_polygonEdges.append(edge);
        } else  {
            m_polygonNodes.append(m_tmpNode);
        }
        m_addingNode = false;
    }
}

QPolygon* GraphicsScene::detectionAreaPolygon() {
    QPolygon* ret = new QPolygon();
    for (int i = 0; i < m_polygonNodes.count(); i++) {
        ret->append(m_polygonNodes.at(i)->pos().toPoint());
    }
    return ret;
}

bool GraphicsScene::connectDots()
{
    if (m_polygonNodes.size() > 2) {
        PolygonNode* firstNode = m_polygonNodes.first();
        PolygonNode* lastNode = m_polygonNodes.last();
        PolygonEdge* edge = new PolygonEdge(lastNode, firstNode);
        this->addItem(edge);
        m_polygonEdges.append(edge);
        m_polygonClosed = true;
        return true;
    }
    return false;
}

void GraphicsScene::clearPoly()
{
    m_polygonClosed = false;

    while (!m_polygonNodes.isEmpty()) {
        PolygonNode* node = m_polygonNodes.takeLast();
        delete node;
    }
    while (!m_polygonEdges.isEmpty()) {
        PolygonEdge* edge = m_polygonEdges.takeLast();
        delete edge;
    }
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
    clearPoly();
    qDebug() << "deleted scene";
}

