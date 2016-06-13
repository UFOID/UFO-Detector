#ifndef VIDEOCODECSUPPORTINFO_H
#define VIDEOCODECSUPPORTINFO_H

#include "config.h"
#include <QObject>
#include <QProcess>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


/**
 * @brief Support info about video codecs.
 *
 * @see http://www.fourcc.org/codecs.php
 */
class VideoCodecSupportInfo : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Support providers.
     * @note Encoder executable location is specified in the settings (Config class).
     */
    enum SupportProviders {
        OpenCv = 0,
        Encoder,
        SUPPORT_PROVIDER_COUNT
    };

    explicit VideoCodecSupportInfo(Config* config, QObject *parent = 0);

    /**
     * @brief This method does the work for actually collecting info about supported codecs.
     */
    void init();

    /**
     * @brief Convert FOURCC string to FOURCC code.
     * @param fourccStr FOURCC code as string
     * @return FOURCC code
     */
    static int toFourcc(QString fourccStr);

    /**
     * @brief Whether OpenCV supports the specified codec.
     * @param fourcc FOURCC code for codec
     * @return true: codec supported, false: not supported
     */
    bool isOpencvSupported(int fourcc);

    /**
     * @brief Whether the video encoder supports the specified codec.
     * @param fourcc FOURCC code for codec
     * @return true: codec supported, false: not supported
     */
    bool isEncoderSupported(int fourcc);

    /**
     * @brief List of codecs with some kind of support. Codecs are listed as FOURCC codes.
     */
    QList<int> supportedCodecs();

#ifndef _UNIT_TEST_
private:
#endif
    Config* m_config;
    QHash<int, QList<int>> m_codecSupport;  ///< encoder fourcc -> support provider list
    QMap<int, QString> m_fourccToEncoderStr;    ///< encoder fourcc -> encoder ID string used by encoder

    QString m_testFileName;     ///< file name used in support tests -- @todo must not collide with existing files

    /**
     * @brief Test whether OpenCV supports the specified codec.
     * @param fourcc FOURCC code for codec
     * @return
     */
    bool testOpencvSupport(int fourcc);

    /**
     * @brief Test whether the video encoder supports the specified codec.
     * @param fourccStr FOURCC code for codec
     * @return true: codec supported, false: not supported
     */
    bool testEncoderSupport(int fourcc);

signals:

public slots:
};

#endif // VIDEOCODECSUPPORTINFO_H
