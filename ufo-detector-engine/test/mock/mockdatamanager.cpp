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

#include "datamanager.h"

DataManager::DataManager(Config* config, QObject* parent) : QObject(parent) {
    m_config = config;
}

bool DataManager::readResultDataFile() {
    return true;
}

void DataManager::saveResultData(QString dateTime, QString videoLength) {
    Q_UNUSED(dateTime);
    Q_UNUSED(videoLength);
}

void DataManager::handleUpdateReply(QNetworkReply *reply) {
    Q_UNUSED(reply);
}

void DataManager::handleBirdClassifierReply(QNetworkReply *reply) {
    Q_UNUSED(reply);
}

bool DataManager::readDetectionAreaFile(bool clipToCamera) {
    Q_UNUSED(clipToCamera);
    return true;
}

QList<QPolygon*>& DataManager::detectionArea() {
    return m_detectionAreaPolygons;
}
