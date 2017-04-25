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
#include "datamanager.h"
#include <QString>
#include <QtTest>
#include <QDomDocument>
#include <QDomNode>

class TestDataManager : public QObject
{
    Q_OBJECT

public:
    TestDataManager();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void constructor();
    void initDataManager();
    void resultDataDomDocument();
    void removeVideo();
    void saveResultData();
    void checkFolders();
    void checkDetectionAreaFile();
    void readResultDataFile();
    void readDetectionAreaFile();
    void detectionArea();
    void resetDetectionAreaFile();

public slots:
    void onResultDataSaved(QString dirName, QString dateTime, QString videoLength);

private:
    QFile m_resultDataFile;
    int m_resultDataSavedCounter;
    Config* m_config;
    DataManager* m_dataManager;
};

TestDataManager::TestDataManager()
{
}

void TestDataManager::initTestCase() {
    // create result data file here because it is created in MainWindow and not by Recorder
    m_resultDataFile.setFileName(m_config->resultDataFile());
    if (m_resultDataFile.exists()) {
        m_resultDataFile.remove();
    }
    QDomDocument resultDataDom;
    QVERIFY(!m_resultDataFile.exists());
    QVERIFY(m_resultDataFile.open(QFile::ReadWrite));
    QTextStream stream(&m_resultDataFile);
    QDomElement ufoidElement = resultDataDom.createElement("UFOID");
    resultDataDom.appendChild(ufoidElement);
    stream << resultDataDom.toString();
    m_resultDataFile.flush();
    m_resultDataFile.close();
    QVERIFY(m_resultDataFile.exists());
    m_config = new Config();
    m_dataManager = new DataManager(m_config);
}

void TestDataManager::cleanupTestCase() {
    QVERIFY(m_resultDataFile.remove());
    QVERIFY(!m_resultDataFile.exists());
    QDir videoDir(m_config->resultVideoDir());
    QVERIFY(videoDir.removeRecursively());
    QVERIFY(!videoDir.exists());
    m_dataManager->deleteLater();
    m_config->deleteLater();
}

void TestDataManager::init() {
    m_resultDataSavedCounter = 0;
}

void TestDataManager::cleanup() {
}

void TestDataManager::onResultDataSaved(QString dirName, QString dateTime, QString videoLength) {
    Q_UNUSED(dirName);
    Q_UNUSED(dateTime);
    Q_UNUSED(videoLength);
    m_resultDataSavedCounter++;
}

void TestDataManager::constructor()
{
    QVERIFY(m_config == m_dataManager->m_config);
    QVERIFY(false == m_dataManager->m_initialized);
    QVERIFY(APPLICATION_VERSION == m_dataManager->m_applicationVersion);
}

void TestDataManager::initDataManager() {
    QSKIP("TODO");
}

void TestDataManager::resultDataDomDocument() {
    QSKIP("TODO");
}

void TestDataManager::removeVideo() {
    QSKIP("TODO");
}

void TestDataManager::saveResultData() {
    QString dateTime = "2017-04-10--12-00-00";
    QString videoLength = "01:02";
    QDomDocument resultDataDom;
    QDomElement videoEntry;
    connect(m_dataManager, SIGNAL(resultDataSaved(QString,QString,QString)),
            this, SLOT(onResultDataSaved(QString,QString,QString)));

    // case: write one entry

    // check result data file doesn't have any other content than the main tag
    QVERIFY(m_resultDataFile.open(QFile::ReadOnly));
    QVERIFY(resultDataDom.setContent(m_resultDataFile.readAll()));
    m_resultDataFile.close();
    QVERIFY(resultDataDom.firstChild().childNodes().isEmpty());
    resultDataDom.clear();

    QCOMPARE(m_resultDataSavedCounter, 0);

    m_dataManager->init();

    m_dataManager->saveResultData(dateTime, videoLength);

    QCOMPARE(m_resultDataSavedCounter, 1);
    QVERIFY(!m_dataManager->m_resultDataFile.isOpen());
    // check content of result data file
    QVERIFY(m_resultDataFile.open(QFile::ReadOnly));
    QVERIFY(resultDataDom.setContent(m_resultDataFile.readAll()));
    m_resultDataFile.close();
    QVERIFY(!resultDataDom.firstChild().childNodes().isEmpty());
    QCOMPARE(resultDataDom.firstChild().childNodes().count(), 1);
    videoEntry = resultDataDom.firstChild().childNodes().at(0).toElement();
    QCOMPARE(videoEntry.nodeName(), QString("Video"));
    QCOMPARE(videoEntry.attributes().length(), 3);
    QCOMPARE(videoEntry.attribute("Pathname"), m_config->resultVideoDir());
    QCOMPARE(videoEntry.attribute("Length"), videoLength);
    QCOMPARE(videoEntry.attribute("DateTime"), dateTime);
    resultDataDom.clear();
}

void TestDataManager::checkFolders() {
    QSKIP("TODO");
}

void TestDataManager::checkDetectionAreaFile() {
    QSKIP("TODO");
}

void TestDataManager::readResultDataFile() {
    QSKIP("TODO");
}

void TestDataManager::readDetectionAreaFile() {
    QSKIP("TODO");
}

void TestDataManager::detectionArea() {
    QVERIFY(m_dataManager->detectionArea() == m_dataManager->m_detectionAreaPolygons);
}

void TestDataManager::resetDetectionAreaFile() {
    QSKIP("TODO");
}

QTEST_APPLESS_MAIN(TestDataManager)

#include "testdatamanager.moc"
