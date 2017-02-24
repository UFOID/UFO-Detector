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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "config.h"
#include "camera.h"
#include "detectionareaeditdialog.h"
#include "cameraresolutiondialog.h"
#include <QDialog>
#include <vector>
#include <opencv2/opencv.hpp>
#include <memory>
#include <thread>

class Camera;

namespace Ui {
class SettingsDialog;
}

/**
 * @brief The settings dialog class
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0, Camera* camPtr = 0, Config* configPtr = 0);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;
    Camera* cameraPtr;
    Config* m_config;
    void saveSettings();
    bool m_detectionAreaDialogIsOpen;
    bool wasSaved;
    DetectionAreaEditDialog* m_detectionAreaDialog;
    CameraResolutionDialog* m_resolutionDialog;

    //std::unique_ptr<std::thread> threadXMLfile;
    //void checkAreaFile();


private slots:
    void on_toolButtonVideoPath_clicked();
    void on_buttonSave_clicked();
    void on_buttonCancel_clicked();
    void on_checkBoxsaveImages_stateChanged(int arg1);
    void on_toolButtonDetectionAreaFile_clicked();
    void on_toolButtonImagePath_clicked();
    void on_buttonSelectDetectionArea_clicked();
    void on_toolButtonResolutionDialog_clicked();
    void onResolutionAcceptedInDialog(QSize resolution);
    //void startThreadCheckXML();
    //void cleanupThreadCheckXML();
    void on_toolButtonCodecHelp_clicked();
    void on_toolButtonTokenHelp_clicked();

    void on_checkBoxFilterAiplanes_clicked();

    void on_toolButtonFilterAirplanes_clicked();

public slots:

signals:
    //void finishedCheckingXML();


};

#endif // SETTINGS_H
