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

#include <QApplication>
#include "mainwindow.h"
#include "actualdetector.h"
#include "camera.h"
#include "config.h"
#include <iostream>
#include <QDebug>

int main(int argc, char *argv[])
{
	try 
	{
		QApplication a(argc, argv);

        QFile styleFile(":/detector.qss");
        if (styleFile.open(QFile::ReadOnly))
        {
            QString styleSheet = styleFile.readAll();
            a.setStyleSheet(styleSheet);
            styleFile.close();
        } else {
            qDebug() << "Missing stylesheet file:" << styleFile.fileName();
        }

        Config myConfig;
        Camera myCam(myConfig.cameraIndex(), myConfig.cameraWidth(), myConfig.cameraHeight());
        myCam.init();
        MainWindow mainWindow(0, &myCam, &myConfig);
        ActualDetector detector(&mainWindow, &myCam, &myConfig);
        mainWindow.setSignalsAndSlots(&detector);
        mainWindow.show();

		return a.exec();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
    return -1;
}
