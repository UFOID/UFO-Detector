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

#include "recorder.h"

Recorder::Recorder(Camera* cameraPtr, Config* configPtr) {
    Q_UNUSED(cameraPtr);
    Q_UNUSED(configPtr);
}

void Recorder::startRecording(cv::Mat &firstFrame) {
    Q_UNUSED(firstFrame);
}

void Recorder::stopRecording(bool willSaveVideo) {
    Q_UNUSED(willSaveVideo);
}

void Recorder::setRectangle(cv::Rect &r, bool isRed) {
    Q_UNUSED(r);
    Q_UNUSED(isRed);
}

void Recorder::reloadResultDataFile() {
}

void Recorder::startEncodingVideoToFFV1(QString tempVideoFileName, QString targetVideoFileName) {
    Q_UNUSED(tempVideoFileName);
    Q_UNUSED(targetVideoFileName);
}

void Recorder::onVideoEncodingFinished() {
}
