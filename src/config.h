#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

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
        SETTINGS_COUNT
    };

    explicit Config(QObject *parent = 0);

    /**
     * @brief applicationVersion Application version number
     * @return version string in format major.minor.subminor
     */
    QString applicationVersion();

    /**
     * @brief checkApplicationUpdates whether to automatically check application updates on startup
     * This is a developer setting and needs to be added manually into the settings file.
     * @return true: check, false: don't check
     */
    bool checkApplicationUpdates();

    /**
     * @brief cameraIndex web camera index as defined by OpenCV
     */
    int cameraIndex();

    /**
     * @brief cameraWidth web camera width in pixels
     */
    int cameraWidth();

    /**
     * @brief cameraHeight web camera height in pixels
     */
    int cameraHeight();

    /**
     * @brief checkCameraAspectRatio checking web camera aspect ratio
     * This is a developer setting and needs to be added manually into the settings file.
     * @return true if checking, false if not
     */
    bool checkCameraAspectRatio();

    /**
     * @brief detectionAreaFile full name of file containing detection area definition
     */
    QString detectionAreaFile();

    /**
     * @brief detectionAreaSize size of detection area in pixels -- will be calculated automatically
     * Detection area size will be calculated automatically when defining
     * it in the settings dialog, so don't edit it.
     * @return
     */
    int detectionAreaSize();

    /**
     * @brief noiseFilterPixelSize pixel size for noise filter
     */
    int noiseFilterPixelSize();

    /**
     * @brief motionThreshold threshold value for motion detection
     */
    int motionThreshold();

    /**
     * @brief minPositiveDetections minimum number motion detections to start video recording
     */
    int minPositiveDetections();

    /**
     * @brief birdClassifierTrainingFile bird classifier training data (using cascade classifier)
     */
    QString birdClassifierTrainingFile();

    /**
     * @brief resultDataFile full name of result data file / detection log
     */
    QString resultDataFile();

    /**
     * @brief resultVideoDir directory for result videos
     */
    QString resultVideoDir();

    /**
     * @brief resultVideoCodec result video codec
     * 0: raw video, 1: lossless FFV1, 2: lossless Lagarith
     * @return
     */
    int resultVideoCodec();

    /**
     * @brief resultVideoWithObjectRectangles whether to draw rectangles around objects in result video
     * @return
     */
    bool resultVideoWithObjectRectangles();

    /**
     * @brief videoEncoderLocation video encoder location (ffmpeg, avconv)
     * @return
     */
    QString videoEncoderLocation();

    /**
     * @brief resultImageDir directory for result images
     */
    QString resultImageDir();

    /**
     * @brief saveResultImages whether to save result images
     * @return true: save images, false: don't save images
     */
    bool saveResultImages();

    /**
     * @brief userToken user's credentials at ufoid.net
     * The credentials are available at request from ufoid.net
     * @return
     */
    QString userTokenAtUfoId();

    /**
     * @brief setApplicationVersion set application version. Format: major.minor.subminor
     * @param version
     */
    void setApplicationVersion(QString version);

    /**
     * @brief setCameraIndex set web camera index as used by OpenCV
     * @param index
     */
    void setCameraIndex(int index);

    /**
     * @brief setCameraWidth set web camera width
     * @param width
     */
    void setCameraWidth(int width);

    /**
     * @brief setCameraHeight set web camera height
     * @param height
     */
    void setCameraHeight(int height);

    /**
     * @brief setDetectionAreaFile set full name of detection area file
     * @param fileName
     */
    void setDetectionAreaFile(QString fileName);

    /**
     * @brief resetDetectionAreaFile reset detection area file name to default value
     */
    void resetDetectionAreaFile();

    /**
     * @brief setDetectionAreaSize set detection area size in pixels
     * @param areaSize
     */
    void setDetectionAreaSize(int areaSize);

    /**
     * @brief setNoiseFilterPixelSize set noise filter pixel size
     * @param size
     */
    void setNoiseFilterPixelSize(int size);

    /**
     * @brief setMotionThreshold set motion threshold
     * @param threshold
     */
    void setMotionThreshold(int threshold);

    /**
     * @brief setMinPositiveDetections set minimum number of positive detections to start video recording
     * @param detectionCount
     */
    void setMinPositiveDetections(int detectionCount);

    /**
     * @brief setResultVideoDir set directory for video saving
     * @param dirName
     */
    void setResultVideoDir(QString dirName);

    /**
     * @brief setResultVideoCodec set result video codec
     * @param codec
     */
    void setResultVideoCodec(int codec);

    /**
     * @brief setResultVideoWithObjectRectangles set whether to draw rectangles around objects in result videos
     * @param drawRectangles true: draw rectangles, false: don't draw rectangles
     */
    void setResultVideoWithObjectRectangles(bool drawRectangles);

    /**
     * @brief setResultImageDir set directory for result image saving
     * @param dirName
     */
    void setResultImageDir(QString dirName);

    /**
     * @brief setSaveResultImages save result images
     * @param save true: save, false: don't save
     */
    void setSaveResultImages(bool save);

    /**
     * @brief setUserTokenAtUfoId set user token at ufoid.net
     * @param token
     */
    void setUserTokenAtUfoId(QString token);

#ifndef _UNIT_TEST_
private:
#endif
    QSettings* m_settings;
    QString m_settingKeys[Config::SETTINGS_COUNT]; ///< setting key strings

    QString m_defaultApplicationVersion;    ///< default app version number: empty by default
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
    int m_defaultVideoCodec;
    bool m_defaultResultVideoWithRectangles; ///< whether to draw rectanges into result video
    QString m_defaultVideoEncoderLocation;
    QString m_defaultResultImageDir;    ///< default directory for result images
    bool m_defaultSaveResultImages;     ///< whether to save result images by default

    QString m_defaultUserTokenAtUfoId;  ///< default user token for ufoid.net: empty value by default

signals:
    /**
     * @brief settingsChanged emitted when any of the settings changed
     */
    void settingsChanged();
};

#endif // CONFIG_H
