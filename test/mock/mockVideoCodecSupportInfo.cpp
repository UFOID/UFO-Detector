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

#include "videocodecsupportinfo.h"

VideoCodecSupportInfo::VideoCodecSupportInfo(QString externalVideoEncoderLocation, QObject *parent)
    : QObject(parent)
{
    m_videoEncoderLocation = externalVideoEncoderLocation;
    m_isInitialized = false;
}

bool VideoCodecSupportInfo::init() {
    m_isInitialized = true;
    return true;
}

bool VideoCodecSupportInfo::isInitialized() {
    return m_isInitialized;
}

int VideoCodecSupportInfo::stringToFourcc(QString fourccStr) {
    char* str = fourccStr.toLocal8Bit().data();
    return CV_FOURCC(str[0], str[1], str[2], str[3]);
}

QString VideoCodecSupportInfo::fourccToString(int fourcc) {
    Q_UNUSED(fourcc);
    return "";
}

bool VideoCodecSupportInfo::isOpencvSupported(int fourcc) {
    Q_UNUSED(fourcc);
    return false;
}

bool VideoCodecSupportInfo::isEncoderSupported(int fourcc) {
    Q_UNUSED(fourcc);
    return false;
}

QList<int> VideoCodecSupportInfo::supportedCodecs() {
    QList<int> list;
    return list;
}

QString VideoCodecSupportInfo::codecName(int fourcc) {
    Q_UNUSED(fourcc);
    return "";
}

QString VideoCodecSupportInfo::fourccToEncoderString(int fourcc) {
    Q_UNUSED(fourcc);
    return "";
}

QString VideoCodecSupportInfo::rawVideoCodecStr() {
    return "IYUV";
}

