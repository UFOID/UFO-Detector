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

#ifndef CAMERARESOLUTIONDIALOG_H
#define CAMERARESOLUTIONDIALOG_H

#include "config.h"
#include "camera.h"
#include "camerainfo.h"
#include <QDialog>

namespace Ui {
class CameraResolutionDialog;
}

/**
 * @brief Dialog for querying supported resolutions from camera
 *
 * @todo use cache so that don't need to query again when dialog is reopened; need to check camera is the same
 */
class CameraResolutionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CameraResolutionDialog(Camera* camera, Config* config, QWidget *parent = 0);
    ~CameraResolutionDialog();

signals:
    /**
     * @brief Emitted when user has selected and accepted a resolution and the dialog is closing
     * @param resolution camera resolution
     */
    void resolutionAccepted(QSize resolution);

public slots:
    void on_okPushButton_clicked();
    void on_cancelPushButton_clicked();
    void on_startQueryPushButton_clicked();
    void on_resolutionComboBox_currentIndexChanged(int index);
    void onQueryProgressChanged(int percent);

private:
    Ui::CameraResolutionDialog *ui;
    Config* m_config;
    Camera* m_camera;

    void updateResolutionComboBox();
};

#endif // CAMERARESOLUTIONDIALOG_H
