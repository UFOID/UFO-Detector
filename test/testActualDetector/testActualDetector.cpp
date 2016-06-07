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

#include "actualdetector.h"
#include "mainwindow.h"
#include "config.h"
#include "camera.h"
#include <QString>
#include <QtTest>
#include <QCoreApplication>

class TestActualDetector : public QObject
{
    Q_OBJECT

public:
    TestActualDetector();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void constructor();

private:
    ActualDetector* m_actualDetector;
    MainWindow* m_mainWindow;
    Config* m_config;
    Camera* m_camera;
};

TestActualDetector::TestActualDetector()
{
}

void TestActualDetector::initTestCase()
{
    m_config = new Config();
    QVERIFY(NULL != m_config);
    m_camera = new Camera(m_config->cameraIndex(), m_config->cameraWidth(), m_config->cameraHeight());
    QVERIFY(NULL != m_camera);
    m_mainWindow = new MainWindow(NULL, m_camera, m_config);
    QVERIFY(NULL != m_mainWindow);
    m_actualDetector = new ActualDetector(m_mainWindow, m_camera, m_config);
    QVERIFY(NULL != m_actualDetector);
}

void TestActualDetector::cleanupTestCase()
{
    m_actualDetector->deleteLater();
    m_mainWindow->deleteLater();
    m_camera->deleteLater();
    m_config->deleteLater();
}

void TestActualDetector::constructor()
{
    QVERIFY(NULL != m_actualDetector->m_mainWindow);
    QVERIFY(NULL != m_actualDetector->m_config);
    QVERIFY(NULL != m_actualDetector->camPtr);
    QVERIFY(m_mainWindow == m_actualDetector->m_mainWindow);
    QVERIFY(m_config == m_actualDetector->m_config);
    QVERIFY(m_camera == m_actualDetector->camPtr);
}

QTEST_MAIN(TestActualDetector)

#include "testactualdetector.moc"
