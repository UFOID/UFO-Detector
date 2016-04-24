#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

/**
 * @brief Global configuration variables
 */
class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = 0);

    QString videoEncoderLocation(); ///< full path to video encoder (ffmpeg, avconv)

signals:

public slots:

private:
    QString m_videoEncoderLocation;
};

#endif // CONFIG_H
