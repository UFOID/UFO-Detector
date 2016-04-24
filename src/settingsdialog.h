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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <vector>
#include <opencv2/opencv.hpp>
#include "dialog.h"
#include <memory>
#include <thread>

class Camera;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0, Camera* camPtr = 0);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;
    Camera* cameraPtr;
    void saveSettings();
    bool dialogIsOpened;
    bool wasSaved;
    Dialog* myDialog;
    //std::unique_ptr<std::thread> threadXMLfile;
    QString xmlFile;
    //void checkAreaFile();


private slots:
    void on_toolButtonVideoPath_clicked();
    void on_buttonSave_clicked();
    void on_buttonCancel_clicked();
    void on_checkBoxsaveImages_stateChanged(int arg1);
    void on_toolButtonXMLfile_clicked();
    void on_toolButtonImagePath_clicked();
    void on_buttonXML_clicked();
    //void startThreadCheckXML();
    //void cleanupThreadCheckXML();
    void on_toolButton_clicked();
    void on_toolButtonToken_clicked();

public slots:

signals:
    //void finishedCheckingXML();


};

#endif // SETTINGS_H
