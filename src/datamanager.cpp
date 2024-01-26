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

#include "datamanager.h"

DataManager::DataManager(Config *config, QObject *parent) : QObject(parent) {
    m_config = config;
    m_initialized = false;
    m_applicationVersion = APPLICATION_VERSION;
}

bool DataManager::init() {
    bool ok = false;
    bool allOk = true;

    m_resultDataFile.setFileName(m_config->resultDataFile());

    ok = checkFolders();
    if (!ok) {
        allOk = false;
    }

    ok = checkDetectionAreaFile();
    if (!ok) {
        allOk = false;
    }

    ok = readResultDataFile();
    if (!ok) {
        allOk = false;
    }

    m_initialized = allOk;
    return allOk;
}

bool DataManager::checkFolders() {
    QDir videoFolder(m_config->resultVideoDir());
    QFileInfo detectionAreaFileInfo(m_config->detectionAreaFile());
    QFileInfo resultDataFileInfo(m_config->resultDataFile());
    QFileInfo birdTrainingDataFileInfo(m_config->birdClassifierTrainingFile());
    QDir detectionAreaFileFolder(detectionAreaFileInfo.absoluteDir());
    QDir resultDataFileFolder(resultDataFileInfo.absoluteDir());
    QDir birdTrainingDataFileFolder(birdTrainingDataFileInfo.absoluteDir());
    bool ok = false;
    bool allOk = true;

    if (!(videoFolder.exists() && !m_config->resultVideoDir().isEmpty())) {
        qDebug() << "creating video and image folders";
        ok = videoFolder.mkpath(m_config->resultImageDir());
        if (!ok) {
            allOk = false;
        }
        ok = videoFolder.mkpath(m_config->resultVideoDir());
        if (!ok) {
            allOk = false;
        }
    }

    if (!detectionAreaFileFolder.exists() && !detectionAreaFileFolder.absolutePath().isEmpty()) {
        qDebug() << "Creating folder for detection area file:" << detectionAreaFileFolder.absolutePath();
        ok = detectionAreaFileFolder.mkpath(detectionAreaFileFolder.absolutePath());
        if (!ok) {
            allOk = false;
        }
    }

    if (!resultDataFileFolder.exists() && !resultDataFileFolder.absolutePath().isEmpty()) {
        qDebug() << "Creating folder for result data file:" << resultDataFileFolder.absolutePath();
        ok = resultDataFileFolder.mkpath(resultDataFileFolder.absolutePath());
        if (!ok) {
            allOk = false;
        }
    }

    if (!birdTrainingDataFileFolder.exists() && !birdTrainingDataFileFolder.absolutePath().isEmpty()) {
        qDebug() << "Creating folder for bird classifier training data file:"
                 << birdTrainingDataFileFolder.absolutePath();
        ok = birdTrainingDataFileFolder.mkpath(birdTrainingDataFileFolder.absolutePath());
        if (!ok) {
            allOk = false;
        }
    }
    return allOk;
}

bool DataManager::checkDetectionAreaFile() {
    QFile areaFile(m_config->detectionAreaFile());
    if (areaFile.exists()) {
        qDebug() << "DataManager: found detection area file";
        return true;
    } else {
        qDebug() << "DataManager: couldn't find detection area file";
        if (areaFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "DataManager: created detection area file " << areaFile.fileName();
            areaFile.close();
            return true;
        }
    }
    qDebug() << "DataManager: failed to create detection area file " << areaFile.fileName();
    return false;
}


bool DataManager::readResultDataFile() {
    bool ok = false;
    if (m_resultDataFile.exists()) {
        /// @todo only update result data file periodically, no need to read/write all the time?
        ok = m_resultDataFile.open(QIODevice::ReadOnly | QIODevice::Text);
        if (!ok) {
            qDebug() << "DataManager: failed to read the result data file" << m_resultDataFile.fileName();
        } else {
//            ParseResult result = m_resultDataDomDocument.setContent(&m_resultDataFile);
//            if (!ok) {
//                qDebug() << "DataManager: failed to load the result data file" << m_resultDataFile.fileName();
//            } else {
//                qDebug() << "Correctly loaded root element from result data file";
//            }
            m_resultDataFile.close();
        }
        return ok;
    } else {
        ok = m_resultDataFile.open(QIODevice::WriteOnly | QIODevice::Text);
        if (!ok) {
            qDebug() << "Failed to create result data file" << m_resultDataFile.fileName();
        } else {
            qDebug() << "Creating result data file" << m_resultDataFile.fileName();
            QDomDocument tempFirstTime;
            tempFirstTime.appendChild(tempFirstTime.createElement("UFOID"));
            QTextStream stream(&m_resultDataFile);
            stream << tempFirstTime.toString();
            m_resultDataFile.close();
            /// @todo refactor result data file creation and reading into separate methods
            return readResultDataFile();
        }
    }
    return false;
}

QDomDocument *DataManager::resultDataDomDocument(bool readFile) {
    if (readFile) {
        readResultDataFile();
    }
    return &m_resultDataDomDocument;
}

bool DataManager::removeVideo(QString dateTime) {
    QDomNode node = m_resultDataDomDocument.firstChildElement().firstChild();
    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement element = node.toElement();
            QString dateInXML = element.attribute("DateTime");
            if (dateInXML.compare(dateTime) == 0) {
                m_resultDataDomDocument.firstChildElement().removeChild(node);
                if (!m_resultDataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    qDebug() << "Failed to open result data file for item deletion";
                    return false;
                } else {
                    QTextStream stream(&m_resultDataFile);
//                  stream.setCodec("UTF-8");
                    stream << m_resultDataDomDocument.toString();
                    m_resultDataFile.close();
                }
                break;
            }
        }
        node = node.nextSibling();
    }
    return true;
}


void DataManager::saveResultData(QString dateTime, QString videoLength) {
    /// @todo create root element here if it doesn't exist
    QDomElement rootElement = m_resultDataDomDocument.firstChildElement();
    QDomElement node = m_resultDataDomDocument.createElement("Video");
    node.setAttribute("Pathname", m_config->resultVideoDir());
    node.setAttribute("DateTime", dateTime);
    node.setAttribute("Length", videoLength);
    rootElement.appendChild(node);

    if (!m_resultDataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString errorMsg = tr("DataManager: failed to open result data file %1").arg(m_resultDataFile.fileName());
        emit messageBroadcasted(errorMsg);
        return;
    } else {
        QTextStream stream(&m_resultDataFile);
//        stream.setCodec("UTF-8");
        /// @todo find way to append just the latest entry into the file, this is not scalable
        stream << m_resultDataDomDocument.toString();
        if (stream.status() != QTextStream::Ok) {
            QString errorMsg = tr("DataManager: problem writing to result data file %1").arg(
                    m_resultDataFile.fileName());
            emit messageBroadcasted(errorMsg);
        }
        m_resultDataFile.flush();
        m_resultDataFile.close();
    }
    emit resultDataSaved(m_config->resultVideoDir(), dateTime, videoLength);
}

bool DataManager::readDetectionAreaFile(bool clipToCamera) {
    QFile areaFile(m_config->detectionAreaFile());
    QDomDocument doc;
    QDomElement root;
    int x = 0;
    int y = 0;

    if (!areaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMsg = tr(
                "Failed to open the detection area file %1. Please create it in Settings dialog or manually.").arg(
                areaFile.fileName());
        qWarning() << errorMsg;
        return false;
    }

    if (!doc.setContent(&areaFile)) {
        QString errorMsg = tr("Error reading the detection area file %1").arg(areaFile.fileName());
        areaFile.close();
        qWarning() << errorMsg;
        return false;
    }

    areaFile.close();
    root = doc.documentElement();
    if (root.nodeName() != "detectionarealist") {
        QString errorMsg = tr("Expected <detectionarealist> tag in detection area file but not found");
        qWarning() << errorMsg;

        return false;
    }
    QDomNodeList areaList = root.childNodes();

    if (areaList.count() > 1) {
        QString errorMsg = tr("More than one detection areas defined, combining all together");
        qWarning() << errorMsg;

    }

    for (int i = 0; i < areaList.count(); i++) {
        QDomNode area = areaList.at(i);
        QDomNodeList areaNodes = area.childNodes();

        if (area.nodeName() != "detectionarea") {
            QString errorMsg = tr("Expected <detectionarea> tag in detection area file but not found.");
            qWarning() << errorMsg;

            return false;
        }

        QDomNodeList cameraList = area.toElement().elementsByTagName("camera");

        if (cameraList.count() != 1) {
            QString errorMsg = tr("Expected single <camera> tag in detection area. Assuming camera index 0.");
            qWarning() << errorMsg;

        }


        m_detectionAreaPolygons.clear();

        for (int j = 0; j < areaList.count(); j++) {
            // ... same as before ...

            for (int a = 0; a < areaNodes.count(); a++) {
                QDomNode areaSubNode = areaNodes.at(a);

                if (areaSubNode.nodeName() == "polygon") {
                    QDomNodeList pointList = areaSubNode.childNodes();
                    std::vector<cv::Point> polygon;

                    for (int p = 0; p < pointList.count(); p++) {
                        QDomElement pointElement = pointList.at(p).toElement();
                        if (pointElement.nodeName() == "point") {
                            x = pointElement.attribute("x").toInt();
                            y = pointElement.attribute("y").toInt();
                            polygon.emplace_back(x, y);
                        }
                    }
                    // ... clipping and other operations using OpenCV functions ...
                    m_detectionAreaPolygons.append(polygon);
                }
            }
        }
    }

    return true;
}

QList<std::vector<cv::Point>> &DataManager::detectionArea() {
    return m_detectionAreaPolygons;
}

bool DataManager::resetDetectionAreaFile(bool overwrite) {
    QFile detectionAreaFile(m_config->detectionAreaFile());
    if (detectionAreaFile.exists() && !overwrite) {
        qInfo() << "Detection area file already exists, not overwriting";
        return false;
    } else {
        // create directory if it doesn't exist
        QFileInfo detectionAreaFileInfo(detectionAreaFile);
        QDir detectionAreaFileFolder(detectionAreaFileInfo.absoluteDir());
        if (!detectionAreaFileFolder.exists() && !detectionAreaFileFolder.absolutePath().isEmpty()) {
            qDebug() << "Creating folder for detection area file:" << detectionAreaFileFolder.absolutePath();
            bool ok = detectionAreaFileFolder.mkpath(detectionAreaFileFolder.absolutePath());
            if (!ok) {
                qWarning() << "Failed to create folder for detection area file:"
                           << detectionAreaFileFolder.absolutePath();
                return false;
            }
        }
    }
    if (!detectionAreaFile.open(QFile::WriteOnly | QFile::Truncate)) {
        qInfo() << "Failed to open detection area file for writing";
        return false;
    }
    int id = m_config->cameraIndex();
    int width = m_config->cameraWidth();
    int height = m_config->cameraHeight();
    QTextStream out(&detectionAreaFile);

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << Qt::endl;
    out << "<detectionarealist>" << Qt::endl;
    out << "  <detectionarea>" << Qt::endl;
    out << "    <camera id=\"" << id << "\" width=\"" << width
        << "\" height=\"" << height << "\"/>" << Qt::endl;
    out << "    <polygon>" << Qt::endl;
    out << "      <point x=\"0\" y=\"0\"/>" << Qt::endl;
    out << "      <point x=\"0\" y=\"" << (height - 1) << "\"/>" << Qt::endl;
    out << "      <point x=\"" << (width - 1) << "\" y=\"" << (height - 1) << "\"/>" << Qt::endl;
    out << "      <point x=\"" << (width - 1) << "\" y=\"0\"/>" << Qt::endl;
    out << "    </polygon>" << Qt::endl;
    out << "  </detectionarea>" << Qt::endl;
    out << "</detectionarealist>" << Qt::endl;
    detectionAreaFile.close();
    qInfo() << "Detection area file created" << detectionAreaFile.fileName();
    return true;
}
