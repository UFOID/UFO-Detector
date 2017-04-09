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
#include "datamanager.h"
#include "console.h"
#include <iostream>
#include <QCoreApplication>
#include <csignal>

void handleTerminationSignals(int signal) {
    Q_UNUSED(signal);
    std::cout << "Got termination signal, quitting" << std::endl;
    QCoreApplication::exit(0);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef Q_OS_UNIX
    // killing with 2, 3, and 15 works, 9 doesn't properly
    signal(SIGINT, &handleTerminationSignals);  // signal 2
    signal(SIGQUIT, &handleTerminationSignals); // signal 3
    signal(SIGKILL, &handleTerminationSignals); // signal 9
    signal(SIGTERM, &handleTerminationSignals); // signal 15
#else
    std::cerr << "Warning: termination signals not yet handled in other than Unix" << std::endl;
#endif

    try {
        Config config;
        Camera camera(config.cameraIndex(), config.cameraWidth(), config.cameraHeight());
        if (!camera.init()) {
            std::cerr << "Couldn't initialize web camera, quitting" << std::endl;
            return -1;
        }
        DataManager dataManager(&config, &a);
        if (!dataManager.init()) {
            std::cerr << "Problems in data manager initialization" << std::endl;
        }

        ActualDetector actualDetector(&camera, &config, &dataManager, &a);

        Console console(&config, &actualDetector, &camera, &dataManager, &a);
        console.init();
        a.connect(&a, SIGNAL(aboutToQuit()), &console, SLOT(onApplicationAboutToQuit()));

        if (!console.start()) {
            std::cerr << "Error starting Console" << std::endl;
            return -1;
        }
        return a.exec();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return -1;
}
