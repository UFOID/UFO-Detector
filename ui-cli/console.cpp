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

Console::Console(Config* config, ActualDetector* detector, Camera* camera,
                 DataManager* dataManager, QObject* parent) : QObject(parent)
{
    m_config = config;
    m_actualDetector = detector;
    m_camera = camera;
    m_dataManager = dataManager;
    m_initialized = false;
}

Console::~Console() {
    logMessage("Console quitting");
    m_dataManager->deleteLater();
    m_actualDetector->deleteLater();
    m_camera->deleteLater();
    m_config->deleteLater();
}

bool Console::init() {
    logMessage("Noise filter pixel size: " + QString::number(m_config->noiseFilterPixelSize()));
    logMessage("Motion threshold size: " + QString::number(m_config->motionThreshold()));

    connect(m_actualDetector, SIGNAL(positiveMessage()), this, SLOT(onPositiveMessage()));
    connect(m_actualDetector, SIGNAL(negativeMessage()), this, SLOT(onNegativeMessage()));
    connect(m_actualDetector, SIGNAL(errorReadingDetectionAreaFile()), this, SLOT(onDetectionAreaFileReadError()));
    connect(m_actualDetector->getRecorder(), SIGNAL(resultDataSaved(QString,QString,QString)), this, SLOT(onVideoSaved(QString,QString,QString)));
    connect(m_actualDetector->getRecorder(), SIGNAL(recordingStarted()), this, SLOT(onRecordingStarted()));
    connect(m_actualDetector->getRecorder(), SIGNAL(recordingFinished()), this, SLOT(onRecordingFinished()));
    connect(m_actualDetector, SIGNAL(progressValueChanged(int)), this, SLOT(onDetectorStartProgressChanged(int)));
    connect(m_actualDetector, SIGNAL(broadcastOutputText(QString)), this, SLOT(logMessage(QString)));

    if (m_config->checkAirplanes()) {
        m_planeChecker = new PlaneChecker(this, m_config->coordinates());
        connect(m_planeChecker, SIGNAL(foundNumberOfPlanes(int)), m_actualDetector, SLOT(setAmountOfPlanes(int)));
        connect(m_actualDetector, SIGNAL(checkPlane()), m_planeChecker, SLOT(callApi()));
    }
    m_initialized = true;
    return true;
}

bool Console::start() {
    if (!m_initialized) {
        return false;
    }
    return m_actualDetector->start();
}

void Console::onRecordingStarted() {
}

void Console::onRecordingFinished() {
}

void Console::onDetectorStartProgressChanged(int progress) {
    if (100 == progress) {
        logMessage("Detector started");
    }
}

void Console::logMessage(QString message) {
    std::cout << message.toStdString() << std::endl;
}

void Console::onPositiveMessage() {
}

void Console::onNegativeMessage() {
}

void Console::onDetectionAreaFileReadError() {
    logMessage("Error reading detection area file " + m_config->detectionAreaFile());
}

void Console::onVideoSaved(QString filename, QString dateTime, QString length) {
    Q_UNUSED(filename);
    Q_UNUSED(dateTime);
    Q_UNUSED(length);
}

void Console::onApplicationAboutToQuit() {
    m_actualDetector->stopThread();
}
