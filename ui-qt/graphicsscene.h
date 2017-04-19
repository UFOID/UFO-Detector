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

#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include "camera.h"
#include <QGraphicsScene>
#include <QPoint>
#include <QMouseEvent>
#include <QList>
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <QPainter>
#include <QGraphicsItem>

/**
 * @brief Graphics scene for DetectionAreaEditDialog
 */
class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicsScene(QObject *parent = 0, Camera* camera =0);
    ~GraphicsScene();

    /**
     * @brief Get detection area polygon.
     * @return
     *
     * @todo enable using multiple polygons
     */
    QPolygon* detectionAreaPolygon();

    /**
     * @brief Close polygon by connecting start and end vertices.
     * @return true if connecting was successful, false if not
     */
    bool connectDots();

    void clearPoly();

    /**
     * @brief Take picture with web camera and show it.
     * @return true on success, false on failure (webcam error)
     */
    bool takePicture();

#ifndef _UNIT_TEST_
private:
#endif
    QPolygon pol;
    Camera* m_camera;
    QGraphicsPixmapItem* m_picture;
    QGraphicsPathItem* m_path;
    QGraphicsLineItem* m_tmpLine;   ///< line drawn when moving a newly added node
    bool m_polygonClosed;
    bool m_addingNode;  ///< node is being added to polygon

signals:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public slots:
    private:


};

#endif // GRAPHICSSCENE_H
