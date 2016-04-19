#ifndef MAINWINDOW_H
#define MAINWINDOW_H
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

#include <QMainWindow>
#include "actualdetector.h"
#include <QModelIndex>
#include <QDomDocument>
#include <QFile>
#include <messageupdate.h>


namespace Ui {
class MainWindow;
}

class QNetworkReply;
class Camera;
class Settings;
class QNetworkAccessManager;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, Camera *cameraPtr = 0);
    ~MainWindow();
    void addOutputText(QString msg);
    bool getCheckboxState();
    void setSignalsAndSlots(ActualDetector *ptrDetector);

private:
    Ui::MainWindow *ui;
    Settings *settingsDialog;
    ActualDetector* theDetector;
    MessageUpdate* updateWindow;
    std::atomic<bool> isUpdating;
    std::atomic<bool> isRecording;
    std::atomic<bool> lastWasPositive, lastWasInfo;    
    bool isDetecting;
    bool isValidToken;
    int counterPositive_, counterNegative_, recordingCounter_;
    cv::Mat webcamFrame;
    Camera* CamPtr;
    std::unique_ptr<std::thread> threadWebcam;
    cv::Size displayResolution;

    QDomDocument documentXML;
    QFile fileXML;
    QString programVersion;
    QNetworkAccessManager *manager;

    void updateWebcamFrame();
    bool checkAndSetResolution(const int WIDTH, const int HEIGHT);
    void initializeStylsheet();
    void readXmlAndGetRootElement();
    void checkAreaXML();
    bool checkCameraAndCodec(const int WIDTH, const int HEIGHT, const int CODEC);
    void checkFolders();

signals:
    void elementWasRemoved();
    void updatePixmap(QImage img);


public slots:
    void recordingWasStarted();
    void recordingWasStoped();
    void on_progressBar_valueChanged(int value);
	void update_output_text(QString msg);
    void displayPixmap(QImage img);

private slots:
    void checkToken(QNetworkReply* reply);
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_checkBox_stateChanged(int arg1);
    void on_buttonClear_clicked();
    void on_sliderNoise_sliderMoved(int position);
    void on_settingsButton_clicked();
    void on_recordingTestButton_clicked();
    void playClip();
    void deletingFileAndRemovingItem();
    void createUploadWindow();
    void setPositiveMessage();
    void setNegativeMessage();
    void setErrorReadingXML();
    void updateWidgets(QString filename, QString datetime, QString videoLength);
    void on_aboutButton_clicked();
    void checkForUpdate(QNetworkReply* reply);

    void on_buttonImageExpl_clicked();
    void on_sliderThresh_sliderMoved(int position);
    void on_toolButtonNoise_clicked();
    void on_toolButtonThresh_clicked();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
