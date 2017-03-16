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

#include "config.h"
#include <QTest>

QString mockConfigResultDataDir;
int mockConfigResultVideoCodec;
QString mockConfigResultVideoCodecStr;

Config::Config(QObject *parent) {
    Q_UNUSED(parent);
    mockConfigResultVideoCodec = 0;
    mockConfigResultVideoCodecStr = "";
    QString encoderLocation = "";
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    encoderLocation = "/usr/bin/avconv";
#elif defined(Q_OS_WIN)
    encoderLocation = QCoreApplication::applicationDirPath()+"/ffmpeg.exe";
#endif
    QVERIFY(!encoderLocation.isEmpty());
    m_videoCodecSupportInfo = new VideoCodecSupportInfo(encoderLocation);
    m_videoCodecSupportInfo->init();
}

Config::~Config() {
}

QString Config::applicationVersion() {
    return "";
}

bool Config::checkApplicationUpdates() {
    return false;
}

int Config::cameraIndex() {
    return 0;
}

int Config::cameraWidth() {
    return 1280;
}

int Config::cameraHeight() {
    return 720;
}

bool Config::checkCameraAspectRatio() {
    return false;
}

QString Config::detectionAreaFile() {
    return "detectionarea.xml";
}

int Config::detectionAreaSize() {
    return this->cameraWidth() * this->cameraHeight();
}

int Config::noiseFilterPixelSize() {
    return 2;
}

int Config::motionThreshold() {
    return 10;
}

int Config::minPositiveDetections() {
    return 2;
}

QString Config::birdClassifierTrainingFile() {
    return "classifier.xml";
}

QString Config::resultDataFile() {
    return "./resultdata.xml";
}

QString Config::resultVideoDir() {
    return "./videos";
}

QString Config::resultVideoCodecStr() {
    return mockConfigResultVideoCodecStr;
}

int Config::resultVideoCodec() {
    return mockConfigResultVideoCodec;
}

bool Config::resultVideoWithObjectRectangles() {
    return false;
}

QString Config::videoEncoderLocation() {
    return "/usr/bin/avconv";
}

QString Config::resultImageDir() {
    return "./images";
}

bool Config::saveResultImages() {
    return false;
}

QString Config::userTokenAtUfoId() {
    return "";
}

VideoCodecSupportInfo* Config::videoCodecSupportInfo() {
    return m_videoCodecSupportInfo;
}

void Config::setApplicationVersion(QString version) {
    Q_UNUSED(version);
}

void Config::setCameraIndex(int index) {
    Q_UNUSED(index);
}

void Config::setCameraWidth(int width) {
    Q_UNUSED(width);
}

void Config::setCameraHeight(int height) {
    Q_UNUSED(height);
}

void Config::setDetectionAreaFile(QString fileName) {
    Q_UNUSED(fileName);
}

void Config::resetDetectionAreaFile() {
}

void Config::setDetectionAreaSize(int areaSize) {
    Q_UNUSED(areaSize);
}

void Config::setNoiseFilterPixelSize(int size) {
    Q_UNUSED(size);
}

void Config::setMotionThreshold(int threshold) {
    Q_UNUSED(threshold);
}

void Config::setMinPositiveDetections(int detectionCount) {
    Q_UNUSED(detectionCount);
}

void Config::setResultVideoDir(QString dirName) {
    Q_UNUSED(dirName);
}

void Config::setResultVideoCodec(QString codec) {
    mockConfigResultVideoCodecStr = codec;
}

void Config::setResultVideoWithObjectRectangles(bool drawRectangles) {
    Q_UNUSED(drawRectangles);
}

void Config::setResultImageDir(QString dirName) {
    Q_UNUSED(dirName);
}

void Config::setSaveResultImages(bool save) {
    Q_UNUSED(save);
}

void Config::setUserTokenAtUfoId(QString token) {
    Q_UNUSED(token);
}

