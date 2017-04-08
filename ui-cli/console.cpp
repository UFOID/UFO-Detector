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

#include "console.h"

Console::Console(Config* config, ActualDetector* detector, QObject* parent) : QObject(parent) {
    m_config = config;
    m_actualDetector = detector;

    logMessage("Noise filter pixel size: " + m_config->noiseFilterPixelSize());
    logMessage("Motion threshold size: " + m_config->motionThreshold());
}

void Console::setSignalsAndSlots() {
    connect(m_actualDetector, SIGNAL(positiveMessage()), this, SLOT(onPositiveMessage()));
    connect(m_actualDetector, SIGNAL(negativeMessage()), this, SLOT(onNegativeMessage()));
    //connect(m_actualDetector, SIGNAL(errorReadingDetectionAreaFile()), this, SLOT(setErrorReadingDetectionAreaFile()));
    //connect(m_actualDetector->getRecorder(), SIGNAL(resultDataSaved(QString,QString,QString)), this, SLOT(addVideoToVideoList(QString,QString,QString)));
    connect(m_actualDetector->getRecorder(), SIGNAL(recordingStarted()), this, SLOT(onRecordingStarted()));
    connect(m_actualDetector->getRecorder(), SIGNAL(recordingFinished()), this, SLOT(onRecordingFinished()));
    connect(m_actualDetector, SIGNAL(progressValueChanged(int)), this, SLOT(onDetectorStartProgressChanged(int)));
    connect(m_actualDetector, SIGNAL(broadcastOutputText(QString)), this, SLOT(logMessage(QString)));

    if (m_config->checkAirplanes()) {
        m_planeChecker = new PlaneChecker(this, m_config->coordinates());
        connect(m_planeChecker, SIGNAL(foundNumberOfPlanes(int)), m_actualDetector, SLOT(setAmountOfPlanes(int)));
        connect(m_actualDetector, SIGNAL(checkPlane()), m_planeChecker, SLOT(callApi()));
    }
}

void Console::onRecordingStarted() {
    std::cout << "Video recording started" << std::endl;
}

void Console::onRecordingFinished() {
    std::cout << "Video recording finished" << std::endl;
}

void Console::onDetectorStartProgressChanged(int progress) {
    if (100 == progress) {
        std::cout << "Detector started" << std::endl;
    }
}

void Console::logMessage(QString message) {
    std::cout << message.toLocal8Bit().data() << std::endl;
}

void Console::onPositiveMessage() {
    logMessage("Positive");
}

void Console::onNegativeMessage() {
    logMessage("Negative");
}
