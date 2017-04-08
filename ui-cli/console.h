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

#ifndef CONSOLE_H
#define CONSOLE_H

#include "config.h"
#include "actualdetector.h"
#include "recorder.h"
#include "planechecker.h"
#include <QObject>

/**
 * @brief Text-based user interface for UFO Detector
 */
class Console : public QObject
{
    Q_OBJECT
public:
    explicit Console(Config* config, ActualDetector* detector, QObject *parent = 0);

    void setSignalsAndSlots();

#ifndef _UNIT_TEST_
private:
#endif
    Config* m_config;
    ActualDetector* m_actualDetector;
    PlaneChecker* m_planeChecker;

signals:

public slots:
    void onRecordingStarted();
    void onRecordingFinished();
    void onDetectorStartProgressChanged(int progress);
    void logMessage(QString message);   /// @todo move into Logger class (screen, file, forward)
    void onPositiveMessage();
    void onNegativeMessage();
    void onDetectionAreaFileReadError();
    void onVideoSaved(QString filename, QString dateTime, QString length);
};

#endif // CONSOLE_H
