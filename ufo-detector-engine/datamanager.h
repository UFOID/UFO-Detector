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

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "config.h"
#include <QObject>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPolygon>
#include <QList>
#include <queue>

/**
 * @brief Data manager class.
 *
 * @todo move result data saving from Recorder into this class
 */
class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(Config* config, QObject *parent = 0);

    /**
     * @brief Initialize DataManger by checking folders etc.
     * @return true on success, false on failure
     */
    bool init();

    /**
     * @brief Get pointer to DOM document representation of result data file.
     * @param readFile read the result data file, updating the DOM document
     *
     * Don't modify the document. Use removeVideo() to remove a video.
     * Recorder class saves the videos and emits signal after it.
     */
    QDomDocument* resultDataDomDocument(bool readFile = false);

    /**
     * @brief Remove video from result data file and delete associated files.
     * @param dateTime Date and time of video in format "YYYY-MM-DD--hh-mm-ss"
     * @return true on success, false on failure
     */
    bool removeVideo(QString dateTime);

    /**
     * @brief Initiate update check for application and bird classifier data
     *
     * @todo move app update stuff into separate class (UpdateManager)
     */
    void checkForUpdates();

    void saveResultData(QString dateTime, QString videoLength);

    /**
     * @brief Read detection area file.
     * @return true on success, false on failure
     */
    bool readDetectionAreaFile();

    QList<QPolygon*>& detectionArea();

#ifndef _UNIT_TEST_
private:
#endif

    Config* m_config;
    bool m_initialized;
    QString m_applicationVersion;   ///< app version   @todo move into UpdateManager
    QFile m_resultDataFile;  ///< result data file (XML)
    QDomDocument m_resultDataDomDocument;   ///< DOM representation of result data file
    QNetworkAccessManager* m_networkAccessManager;
    QList<QPolygon*> m_detectionAreaPolygons; ///< detection area polygons (cameras not separated)

    /**
     * @brief Check that the folders for images, videos, and other files exist
     */
    bool checkFolders();

    /**
     * @brief Check detection area file and create if doesn't exist
     */
    bool checkDetectionAreaFile();

    /**
     * @brief Initiate download of bird classifier file.
     */
    void downloadBirdClassifierFile();

public slots:
    /**
     * @brief Read result data from file into QDomDocument which can be got with resultDataDomDocument().
     * @return true on success, false on failure
     */
    bool readResultDataFile();

#ifndef _UNIT_TEST_
private slots:
#else
public slots:
#endif

    /**
     * @brief Check for updates for application, bird classifier file.
     * @param reply Update file from http://ufoid.net/version.xml
     */
    void handleUpdateReply(QNetworkReply* reply);

    /**
     * @brief Receive bird classifier file. Download initiated in init() method.
     * @param reply Bird classifier file from http://ufoid.net/cascade.xml
     */
    void handleBirdClassifierReply(QNetworkReply* reply);

signals:
    /**
     * @brief Emitted when new version of UFO Detector is available
     * @param newVersion new version number
     * @param messageInXml messages related to the update
     */
    void newApplicationVersionAvailable(QString newVersion, std::queue<QString> messageInXml);

    /**
     * @brief Emitted when a new video entry was added into result data file.
     * @param videoFolder
     * @param dateTime
     * @param videoLength
     */
    void resultDataSaved(QString videoFolder, QString dateTime, QString videoLength);

    /**
     * @brief Emitted on message broadcast.
     * @param message
     */
    void messageBroadcasted(QString message);
};

#endif // DATAMANAGER_H
