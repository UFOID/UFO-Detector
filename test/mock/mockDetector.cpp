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

#include "Detector.h"
#include <QObject>

CDetector::CDetector(Mat &gray) {
    Q_UNUSED(gray);
}

CDetector::~CDetector() {
}

pair<vector<Point2d>, vector<Rect>> CDetector::Detect(Mat &gray, Rect &croppedRect) {
    Q_UNUSED(gray);
    Q_UNUSED(croppedRect);
}
