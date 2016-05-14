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

#include <QGraphicsScene>
#include <QPoint>
#include <QMouseEvent>
#include <opencv2/imgproc/imgproc.hpp>

class Camera;

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphicsScene(QObject *parent = 0, Camera* camPtr =0);
    ~GraphicsScene();
    std::vector<cv::Point2f> getCoor();
    void connectDots();
    void clearPoly();

private:
    QPolygon pol;

signals:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

public slots:
    private:


};

#endif // GRAPHICSSCENE_H
