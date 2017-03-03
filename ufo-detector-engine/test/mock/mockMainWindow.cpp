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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Camera *cameraPtr, Config *configPtr) {
    Q_UNUSED(parent);
    Q_UNUSED(cameraPtr);
    m_config = configPtr;
}

MainWindow::~MainWindow() {
}

void MainWindow::addOutputText(QString msg) {
    Q_UNUSED(msg);
}


void MainWindow::setSignalsAndSlots(ActualDetector *actualDetector) {
    Q_UNUSED(actualDetector);
}


void MainWindow::onRecordingStarted() {
}

void MainWindow::onRecordingFinished() {
}

void MainWindow::on_progressBar_valueChanged(int value) {
    Q_UNUSED(value);
}

void MainWindow::update_output_text(QString msg) {
    Q_UNUSED(msg);
}

void MainWindow::displayPixmap(QImage img) {
    Q_UNUSED(img);
}

void MainWindow::on_startButton_clicked() { }
void MainWindow::on_checkBoxDisplayWebcam_stateChanged(int arg1) { Q_UNUSED(arg1); }
void MainWindow::on_buttonClear_clicked() { }
void MainWindow::on_sliderNoise_sliderMoved(int position) { Q_UNUSED(position); }
void MainWindow::on_settingsButton_clicked() { }
void MainWindow::on_recordingTestButton_clicked() { }
void MainWindow::onVideoPlayClicked() { }
void MainWindow::onVideoDeleteClicked() { }
void MainWindow::onVideoUploadClicked() { }
void MainWindow::onVideoListContextMenuRequested(const QPoint& pos) { Q_UNUSED(pos); }
void MainWindow::onDeleteSelectedVideosClicked() { }
void MainWindow::setPositiveMessage() { }
void MainWindow::setNegativeMessage() { }
void MainWindow::setErrorReadingDetectionAreaFile() { }
void MainWindow::addVideoToVideoList(QString filename, QString datetime, QString videoLength) {
    Q_UNUSED(filename);
    Q_UNUSED(datetime);
    Q_UNUSED(videoLength);
}
void MainWindow::downloadClassifier(QNetworkReply* reply){ Q_UNUSED(reply); }
void MainWindow::on_aboutButton_clicked() { }
void MainWindow::checkForUpdate(QNetworkReply* reply) { Q_UNUSED(reply); }
void MainWindow::on_buttonImageExpl_clicked() { }
void MainWindow::on_sliderThresh_sliderMoved(int position) { Q_UNUSED(position); }
void MainWindow::on_toolButtonNoise_clicked() { }
void MainWindow::on_toolButtonThresh_clicked() { }
void MainWindow::showEvent(QShowEvent *event) { Q_UNUSED(event); }
void MainWindow::resizeEvent(QResizeEvent *event) { Q_UNUSED(event); }
void MainWindow::closeEvent(QCloseEvent *event) { Q_UNUSED(event); }
