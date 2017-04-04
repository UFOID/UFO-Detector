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
#include "camera.h"
#include "actualdetector.h"
#include "console.h"
#include <iostream>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    try {
        Config config;
        Camera camera(config.cameraIndex(), config.cameraWidth(), config.cameraHeight());
        if (!camera.init()) {
            std::cerr << "Couldn't initialize web camera, quitting" << std::endl;
            return -1;
        }
        ActualDetector actualDetector(&camera, &config, NULL);
        Console console(&config, &actualDetector);
        console.setSignalsAndSlots();

        if (!actualDetector.start()) {
            std::cerr << "Error starting detector" << std::endl;
            return -1;
        }

        std::cout << "Running main loop" << std::endl;
        return a.exec();

        actualDetector.stopThread();

    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return -1;
}
