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

#include "Ctracker.h"
#include <QObject>


CTracker::CTracker(float _dt, float _Accel_noise_mag, double _dist_thres,
    int _maximum_allowed_skipped_frames, int _max_trace_length)
{
    Q_UNUSED(_dt);
    Q_UNUSED(_Accel_noise_mag);
    Q_UNUSED(_dist_thres);
    Q_UNUSED(_maximum_allowed_skipped_frames);
    Q_UNUSED(_max_trace_length);
}

CTracker::~CTracker() {
}

void CTracker::Update(vector<Point2d> &detections) {
    Q_UNUSED(detections);
}

void CTracker::updateEmpty() {
}
