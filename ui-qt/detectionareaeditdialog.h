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

#ifndef DIALOG_H
#define DIALOG_H

#include "config.h"
#include "graphicsscene.h"
#include "datamanager.h"
#include <QDialog>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDir>
#include <QtXml>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include <iostream>

using namespace cv;
using namespace std;

class Camera;

namespace Ui {
class DetectionAreaEditDialog;
}

/**
 * @brief Dialog for the detection area selection
 *
 * @todo save detection area as polygon: only save the corner points. Then can edit, re-edit, scale etc. more freely.
 */
class DetectionAreaEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetectionAreaEditDialog(QWidget *parent = 0, Camera* camera = 0,
        Config* config = 0, DataManager* dataManager = 0);
    ~DetectionAreaEditDialog();

private slots:
    void on_buttonTakePicture_clicked();
    void on_buttonSave_clicked();
    void on_buttonConnect_clicked();
    void on_buttonClear_clicked();

private:
    Ui::DetectionAreaEditDialog *ui;
    Camera *m_camera;
    Config *m_config;
    DataManager* m_dataManager;
    GraphicsScene *m_scene;
    bool m_pictureTaken;
    bool m_isFirstPicture;

    /**
     * @brief Save detection area polygons into XML file.
     * @return true on success, false on failure
     */
    bool savePolygonsAsXml();

    /**
     * @brief Check that polygon is acceptable as detection area polygon: area is larger than 1.
     * @param polygon
     * @return true if polygon is valid, false if not
     */
    bool checkPolygon(QPolygon* polygon);

    /**
     * @brief Read existing detection area polygons from file.
     * @return true on success, false on failure
     */
    bool readPolygonsFromFile();
};

#endif // DIALOG_H
