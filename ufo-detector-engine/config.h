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

#ifndef CONFIG_H
#define CONFIG_H

#include "videocodecsupportinfo.h"
#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#define APPLICATION_VERSION "0.7.0"

/**
 * @brief Global configuration variables for UFO-Detector
 *
 * @todo ensure all keys are in the settings file
 */
class Config : public QObject
{
    Q_OBJECT
public:

    enum SettingKeys {
        ApplicationVersion = 0, // application settings
        CheckApplicationUpdates,
        CameraIndex,        // camera
        CameraWidth,
        CameraHeight,
        CheckCameraAspectRatio,
        DetectionAreaFile,  // detection parameters
        DetectionAreaSize,
        NoiseFilterPixelSize,
        MotionThreshold,
        MinPositiveDetections,
        BirdClassifierTrainingFile,
        ResultDataFile,     // detection results
        ResultVideoDir,
        ResultVideoCodec,
        ResultVideoWithObjectRectangles,
        VideoEncoderLocation,
        ResultImageDir,
        SaveResultImages,
        UserTokenAtUfoId,   // sharing results
        ClassifierVersion,
        CheckAirplanes,
        Coordinates,
        SETTINGS_COUNT
    };

    explicit Config(QObject *parent = 0);

    ~Config();

    /**
     * @brief Application version number.
     * @return version string in format major.minor.subminor
     */
    QString applicationVersion();

    int classifierVersion();

    /**
     * @brief Whether to automatically check application updates on startup.
     * This is a developer setting and needs to be added manually into the settings file.
     * @return true: check, false: don't check
     */
    bool checkApplicationUpdates();

    /**
     * @brief Web camera index as defined by OpenCV.
     */
    int cameraIndex();

    /**
     * @brief Web camera width in pixels.
     */
    int cameraWidth();

    /**
     * @brief Web camera height in pixels.
     */
    int cameraHeight();

    /**
     * @brief Whether to check web camera aspect ratio.
     * This is a developer setting and needs to be added manually into the settings file.
     * @return true if checking, false if not
     */
    bool checkCameraAspectRatio();

    /**
     * @brief Full name of the file containing detection area definition.
     */
    QString detectionAreaFile();

    /**
     * @brief Size of detection area in pixels. This will be calculated automatically.
     * Detection area size will be calculated automatically when defining
     * it in the settings dialog, so don't edit it.
     * @return
     */
    int detectionAreaSize();

    /**
     * @brief Pixel size for noise filter.
     */
    int noiseFilterPixelSize();

    /**
     * @brief Threshold value for motion detection.
     *
     * @todo explain threshold value
     */
    int motionThreshold();

    /**
     * @brief Minimum number of motion detections to start video recording.
     */
    int minPositiveDetections();

    /**
     * @brief Bird classifier training data (using cascade classifier).
     */
    QString birdClassifierTrainingFile();

    /**
     * @brief Full name of result data file / detection log.
     */
    QString resultDataFile();

    /**
     * @brief Directory where the result videos are saved.
     */
    QString resultVideoDir();

    /**
     * @brief Result video codec string.
     *
     * @return FOURCC code of the codec as string
     */
    QString resultVideoCodecStr();

    /**
     * @brief Result video codec. The value is a FOURCC code.
     *
     * @return FOURCC code of the codec
     */
    int resultVideoCodec();

    /**
     * @brief Whether to draw rectangles around objects in the result video.
     * @return
     */
    bool resultVideoWithObjectRectangles();

    /**
     * @brief Location of video encoder (ffmpeg, avconv).
     * @return
     */
    QString videoEncoderLocation();

    /**
     * @brief Directory for result images.
     */
    QString resultImageDir();

    /**
     * @brief Whether to save result images.
     * @return true: save images, false: don't save images
     */
    bool saveResultImages();

    /**
     * @brief User credentials at ufoid.net.
     * The credentials are available at request from ufoid.net.
     * @return
     */
    QString userTokenAtUfoId();

    /**
     * @brief Whether to check for airplanes.
     * @return true: check, false: don't check
     */
    bool checkAirplanes();

    /**
     * @brief The coordinates to check for airplanes
     * @return
     */
    QString coordinates();
    /**
     * @brief Get video codec support info object. The object has been initialized.
     * @return pointer to initialized VideoCodecSupportInfo
     */
    VideoCodecSupportInfo* videoCodecSupportInfo();

    /**
     * @brief Set application version. Format: major.minor.subminor.
     * @note This is for internal usage.
     * @param version
     */
    void setApplicationVersion(QString version);

    /**
     * @brief Set bird classifer version.
     * @note This is for internal usage.
     * @param version
     */
    void setClassifierVersion(int version);

    /**
     * @brief Set web camera index as used by OpenCV.
     * @param index
     */
    void setCameraIndex(int index);

    /**
     * @brief Set web camera width.
     * The final result depends on the camera. Check the camera documentation and
     * Camera::availableResolutions() for supported resolutions.
     * available resolutions.
     * @param width
     */
    void setCameraWidth(int width);

    /**
     * @brief Set web camera height.
     * The final result depends on the camera. Check the camera documentation and
     * Camera::availableResolutions() for supported resolutions.
     * @param height
     */
    void setCameraHeight(int height);

    /**
     * @brief Set full name of detection area file.
     * @param fileName
     */
    void setDetectionAreaFile(QString fileName);

    /**
     * @brief Reset detection area file name to default value.
     */
    void resetDetectionAreaFile();

    /**
     * @brief Set detection area size in pixels.
     * @param areaSize
     */
    void setDetectionAreaSize(int areaSize);

    /**
     * @brief Set noise filter pixel size.
     * @param size
     */
    void setNoiseFilterPixelSize(int size);

    /**
     * @brief Set motion threshold.
     * @param threshold
     */
    void setMotionThreshold(int threshold);

    /**
     * @brief Set minimum number of positive detections to start video recording.
     * @param detectionCount
     */
    void setMinPositiveDetections(int detectionCount);

    /**
     * @brief Set directory for video saving.
     * @param dirName
     */
    void setResultVideoDir(QString dirName);

    /**
     * @brief Set result video codec.
     * @param codec FOURCC code of the codec as string
     */
    void setResultVideoCodec(QString codec);

    /**
     * @brief Set whether to draw rectangles around objects in result videos.
     * @param drawRectangles true: draw rectangles, false: don't draw rectangles
     */
    void setResultVideoWithObjectRectangles(bool drawRectangles);

    /**
     * @brief Set directory for result image saving.
     * @param dirName
     */
    void setResultImageDir(QString dirName);

    /**
     * @brief Save result images.
     * @param save true: save, false: don't save
     */
    void setSaveResultImages(bool save);

    /**
     * @brief Set user token at ufoid.net.
     * @param token
     */
    void setUserTokenAtUfoId(QString token);

    /**
     * @brief Set if check for airplanes.
     * @param True: check, false: dont check
     */
    void setCheckAirplanes(bool check);

    /**
     * @brief Set the coordinates used by PlaneChecker.
     * @param coordinates
     */
    void setAirplanesCoordinates(QString coordinates);


#ifndef _UNIT_TEST_
private:
#endif
    QSettings* m_settings;
    QString m_settingKeys[Config::SETTINGS_COUNT]; ///< setting key strings

    QString m_defaultApplicationVersion;    ///< default app version number: empty by default
    QString m_defaultClassifierVersion;
    bool m_defaultCheckApplicationUpdates;  ///< default setting for automatic app updates check

    int m_defaultCameraIndex;
    int m_defaultCameraWidth;
    int m_defaultCameraHeight;
    bool m_defaultCheckCameraAspectRatio;

    QString m_defaultDetectionDataDir; ///< default directory for data (detection area and result data / log)
    QString m_defaultDetectionAreaFileName; ///< default file name for detection area file
    int m_defaultDetectionAreaSize;

    int m_defaultNoiseFilterPixelSize;
    int m_defaultMotionThreshold;
    int m_defaultMinPositiveDetections;
    QString m_defaultBirdClassifierFileName;  ///< default file name for bird classifier training data file

    QString m_defaultResultDataFileName; ///< default file name for result data file (result database)
    QString m_defaultResultDocumentDir; ///< default root directory for result images and videos
    QString m_defaultResultVideoDir;    ///< default directory for result videos
    QString m_defaultVideoCodecStr;        ///< default video codec as FOURCC string
    bool m_defaultResultVideoWithRectangles; ///< whether to draw rectanges into result video
    QString m_defaultVideoEncoderLocation;
    QString m_defaultResultImageDir;    ///< default directory for result images
    bool m_defaultSaveResultImages;     ///< whether to save result images by default

    QString m_defaultUserTokenAtUfoId;  ///< default user token for ufoid.net: empty value by default

    VideoCodecSupportInfo* m_videoCodecSupportInfo; ///< info about video codec support

signals:
    /**
     * @brief Emitted when any of the settings changed.
     */
    void settingsChanged();
};

#endif // CONFIG_H
