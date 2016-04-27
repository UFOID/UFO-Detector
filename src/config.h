#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>

/**
 * @brief Global configuration variables
 */
class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = 0);

    QString videoEncoderLocation(); ///< full path to video encoder (ffmpeg, avconv)
    QString detectionAreaFile(); ///< XML file for detection area
    QString birdClassifierTrainingFile(); ///< XML file for bird classifier training data (using cascade classifier)
    QString resultDataFile();   ///< XML file for result data (detection log)
    QString resultVideoDir();   ///< directory for result videos
    QString resultImageDir();   ///< directory for result images
    bool checkApplicationUpdates(); ///< whether to automatically check application updates

    void setDetectionAreaFile(QString fileName);
    void setResultVideoDir(QString dirName);
    void setResultImageDir(QString dirName);

    void resetDetectionAreaFile();  ///< reset detection area file to default value

signals:

public slots:

private:
    QString m_defaultDetectionDataDir; ///< default directory for data (detection area and result data / log)
    QString m_defaultDetectionAreaFileName; ///< default file name for detection area file
    QString m_defaultBirdClassifierFileName;  ///< default file name for bird classifier training data file
    QString m_defaultResultDataFileName; ///< default file name for result data file

    QString m_defaultResultDocumentDir; ///< default root directory for result images and videos
    QString m_defaultResultVideoDir;    ///< default directory for result videos
    QString m_defaultResultImageDir;    ///< default directory for result images

    bool m_defaultCheckApplicationUpdates; ///< default setting for automatic app updates check

    QString m_videoEncoderLocation; /// @todo put video encoder location to settings, too

    QSettings* m_settings;
};

#endif // CONFIG_H
