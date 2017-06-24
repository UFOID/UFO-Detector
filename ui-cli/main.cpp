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
    QCoreApplication::setApplicationName("ufo-detector-cli");
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

#ifdef Q_OS_UNIX
    // killing with 2, 3, and 15 works, 9 doesn't properly
    signal(SIGINT, &handleTerminationSignals);  // signal 2
    signal(SIGQUIT, &handleTerminationSignals); // signal 3
    signal(SIGKILL, &handleTerminationSignals); // signal 9
    signal(SIGTERM, &handleTerminationSignals); // signal 15
#else
    std::cerr << "Warning: termination signals not yet handled in other than Unix" << std::endl;
#endif

    QCommandLineParser cmdLineParser;
    cmdLineParser.addHelpOption();
    cmdLineParser.addVersionOption();

    cmdLineParser.setApplicationDescription(QCoreApplication::translate("ufo-detector-cli",
        "UFO Detector command line application"));

    QCommandLineOption resetConfigFileOption("reset-config-file",
        QCoreApplication::translate("ufo-detector-cli", "Reset configuration file."));
    cmdLineParser.addOption(resetConfigFileOption);

    QCommandLineOption resetDetectionAreaFileOption("reset-area-file",
        QCoreApplication::translate("ufo-detector-cli", "Reset detection area file."));
    cmdLineParser.addOption(resetDetectionAreaFileOption);

    QCommandLineOption listCameraResolutionsOption("list-camera-resolutions",
        QCoreApplication::translate("ufo-detector-cli", "List available web camera resolutions."));
    cmdLineParser.addOption(listCameraResolutionsOption);

    cmdLineParser.process(a);

    bool resetConfigFile = cmdLineParser.isSet(resetConfigFileOption);
    bool resetDetectionAreaFile = cmdLineParser.isSet(resetDetectionAreaFileOption);
    bool listCameraResolutions = cmdLineParser.isSet(listCameraResolutionsOption);

    try {
        Config config;
        if (!config.configFileExists() || resetConfigFile) {
            if (!config.configFileExists()) {
                std::cout << "Configuration file doesn't exist, creating with default values..."  << endl;
            } else if (resetConfigFile) {
                std::cout << "Setting default configuration values..." << endl;
            }
            config.createDefaultConfig(true);
            if (!config.configFileExists()) {
                std::cerr << "Failed to write configuration file, quitting" << endl;
                return -1;
            }
            std::cout << "Configuration file " << config.configFileName().toStdString() << " saved" << endl;
            if (resetConfigFile && !resetDetectionAreaFile && !listCameraResolutions) {
                return 0;
            }
        }

        DataManager dataManager(&config, &a);
        if (resetDetectionAreaFile) {
            if (dataManager.resetDetectionAreaFile(true)) {
                std::cout << "Created detection area file " << config.detectionAreaFile().toStdString() << endl;
            } else {
                std::cerr << "Failed to create detection area file " << config.detectionAreaFile().toStdString() << endl;
                return -1;
            }
            if (!listCameraResolutions) {
                return 0;
            }
        }
        if (!dataManager.init()) {
            std::cerr << "Problems in data manager initialization, continuing anyway" << std::endl;
        }

        Camera camera(config.cameraIndex(), config.cameraWidth(), config.cameraHeight());
        if (!camera.init()) {
            std::cerr << "Couldn't initialize web camera, quitting" << std::endl;
            return -1;
        }
        if (listCameraResolutions) {
            std::cout << "Querying available web camera resolutions, this may take a while..." << endl;
            camera.queryAvailableResolutions();
            if (camera.availableResolutions().size() == 0) {
                std::cout << "No web camera resolutions found" << endl;
            } else {
                std::cout << "Supported web camera resolutions:" << endl;
            }
            QListIterator<QSize> resolutionIt(camera.availableResolutions());
            while (resolutionIt.hasNext()) {
                QSize resolution = resolutionIt.next();
                std::cout << resolution.width() << " x " << resolution.height() << endl;
            }
            return 0;
        }

        ActualDetector actualDetector(&camera, &config, &dataManager, &a);

        Console console(&config, &actualDetector, &camera, &dataManager, &a);
        console.init();
        a.connect(&a, SIGNAL(aboutToQuit()), &console, SLOT(onApplicationAboutToQuit()));

        if (!console.start()) {
            std::cerr << "Error starting Console, quitting" << std::endl;
            return -1;
        }
        return a.exec();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return -1;
}
