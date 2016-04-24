#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QSettings>
#include <QApplication>

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

    void setDetectionAreaFile(QString fileName);

signals:

public slots:

private:
    QSettings* m_settings;
    QString m_videoEncoderLocation; /// @todo put video encoder location to settings, too
};

#endif // CONFIG_H
