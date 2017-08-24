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
#include "logger.h"
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
    bool optionsCauseQuit = resetConfigFile || resetDetectionAreaFile || listCameraResolutions;

    try {
        Logger logger;
        logger.setOutputToFileEnabled(false);
        logger.setOutputToStdioEnabled(true);
        logger.setTimestampEnabled(!optionsCauseQuit);

        Config config;
        if (!config.configFileExists() || resetConfigFile) {
            if (!config.configFileExists()) {
                logger.print(QCoreApplication::translate("ufo-detector-cli", "Configuration file doesn't exist, creating with default values..."));
            } else if (resetConfigFile) {
                logger.print(QCoreApplication::translate("ufo-detector-cli", "Setting default configuration values..."));
            }
            config.createDefaultConfig(true);
            if (!config.configFileExists()) {
                logger.print(QCoreApplication::translate("ufo-detector-cli", "Failed to write configuration file, quitting"));
                return -1;
            }
            logger.print(QCoreApplication::translate("ufo-detector-cli", "Configuration file %1 saved").arg(config.configFileName()));
            if (resetConfigFile && !resetDetectionAreaFile && !listCameraResolutions) {
                return 0;
            }
        }
        QFileInfo logFileInfo(config.logFileName());
        QString logFileName = logFileInfo.absolutePath() + QDir::separator() + logFileInfo.baseName()
                + "-cli." + logFileInfo.suffix();
        logger.print("Logging to file " + logFileName);
        logger.setFileName(logFileName);
        logger.setOutputToFileEnabled(true);
        logger.print("ufo-detector-cli starting");

        DataManager dataManager(&config, &a);
        if (resetDetectionAreaFile) {
            if (dataManager.resetDetectionAreaFile(true)) {
                logger.print(QCoreApplication::translate("ufo-detector-cli", "Created detection area file %1").arg(config.detectionAreaFile()));
            } else {
                logger.print(QCoreApplication::translate("ufo-detector-cli", "Failed to create detection area file %1").arg(config.detectionAreaFile()));
                return -1;
            }
            if (!listCameraResolutions) {
                return 0;
            }
        }
        if (!dataManager.init()) {
            logger.print(QCoreApplication::translate("ufo-detector-cli", "Problems in data manager initialization, continuing anyway"));
        }

        Camera camera(config.cameraIndex(), config.cameraWidth(), config.cameraHeight());
        if (!camera.init()) {
            logger.print(QCoreApplication::translate("ufo-detector-cli", "Couldn't initialize web camera, quitting"));
            return -1;
        }
        if (listCameraResolutions) {
            logger.print(QCoreApplication::translate("ufo-detector-cli", "Querying available web camera resolutions, this may take a while..."));
            camera.queryAvailableResolutions();
            if (camera.availableResolutions().size() == 0) {
                logger.print(QCoreApplication::translate("ufo-detector-cli", "No web camera resolutions found"));
            } else {
                logger.print(QCoreApplication::translate("ufo-detector-cli", "Supported web camera resolutions:"));
            }
            QListIterator<QSize> resolutionIt(camera.availableResolutions());
            while (resolutionIt.hasNext()) {
                QSize resolution = resolutionIt.next();
                logger.print(QCoreApplication::translate("ufo-detector-cli", "%1 x %2", "resolution: width x height").arg(resolution.width()).arg(resolution.height()));
            }
            return 0;
        }

        ActualDetector actualDetector(&camera, &config, &logger, &dataManager, &a);

        Console console(&config, &logger, &actualDetector, &camera, &dataManager, &a);
        console.init();
        a.connect(&a, SIGNAL(aboutToQuit()), &console, SLOT(onApplicationAboutToQuit()));

        if (!console.start()) {
            logger.print(QCoreApplication::translate("ufo-detector-cli", "Error starting Console, quitting"));
            return -1;
        }
        return a.exec();
    } catch (std::exception &e) {
        std::cerr << "Caught exception: " << e.what();
    }
    return -1;
}
