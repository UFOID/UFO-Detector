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

#ifndef VIDEOCODECSUPPORTINFO_H
#define VIDEOCODECSUPPORTINFO_H

#include <QObject>
#include <QProcess>
#include <QMap>
#include <QHash>
#include <QFile>
#include <QDebug>
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
     * @brief Video encoders.
     */
    enum VideoEncoders {
        OpenCv = 0,     ///< OpenCV internal encoder (might be same as external, in fact)
        External,       ///< External encoder (ffmpeg/avconv). Executable location specified in settings (Config class)
        VIDEO_ENCODER_COUNT
    };

    /**
     * @brief VideoCodecSupportInfo constructor.
     * @param externalVideoEncoderLocation full path to external video encoder
     * @param parent
     */
    explicit VideoCodecSupportInfo(QString externalVideoEncoderLocation, QObject *parent = 0);

    /**
     * @brief This method does the work for actually collecting info about supported codecs.
     */
    bool init();

    /**
     * @brief Whether this object is initialized.
     */
    bool isInitialized();

    /**
     * @brief Convert FOURCC string to FOURCC code.
     * @param fourccStr FOURCC code as string
     * @return FOURCC code
     */
    int stringToFourcc(QString fourccStr);

    /**
     * @brief Convert FOURCC code to FOURCC string.
     * @param fourcc
     * @return
     */
    QString fourccToString(int fourcc);

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

    /**
     * @brief Name of codec to be shown to user
     * @param fourcc FOURCC code of the codec
     * @return codec name
     */
    QString codecName(int fourcc);

    /**
     * @brief Convert FOURCC code to string used by external encoder.
     * @param fourcc FOURCC code of the codec
     * @return Codec string used by external encoder
     */
    QString fourccToEncoderString(int fourcc);

    /**
     * @brief Shortcut to get raw video codec string
     * @return raw video codec as FOURCC string ("IYUV")
     */
    QString rawVideoCodecStr();

#ifndef _UNIT_TEST_
private:
#endif
    QString m_videoEncoderLocation; ///< video encoder executable location
    bool m_isInitialized;           ///< object initialized
    /// @todo table model would be clearer. Columns: fourcc, encoderStr, codecName, encoderList
    QMap<int, QList<int>> m_codecSupport;      ///< encoder fourcc -> support provider list
    QHash<int, QString> m_fourccToEncoderStr;   ///< encoder fourcc -> encoder ID string used by encoder
    QHash<int, QString> m_fourccToCodecName;    ///< clear text name of codec (max. few words)

    QString m_testFileName;     ///< file name used in support tests -- @todo must not collide with existing files
    QString m_rawVideoCodecStr; ///< raw video codec FOURCC string

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
