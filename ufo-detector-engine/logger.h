#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QDateTime>
#include <iostream>
#include <QDebug>

#define LOGGER_TIMESTAMP_FORMAT_QT "yyyy-MM-dd hh:mm:ss.zzz"

/**
 * @brief The logger class to print messages to the log file and the screen.
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = 0);
    ~Logger();

    /**
     * @brief Set file name of the log file.
     * @param fileName
     */
    void setFileName(QString fileName);

    /**
     * @brief File name of the log file.
     * @return
     */
    QString fileName();

    /**
     * @brief Enable or disable outputting log messages to the log file.
     * @param enabled
     */
    void setOutputToFileEnabled(bool enabled = true);

    /**
     * @brief Enable or disable outputting log messages to standard output/error.
     * Currently only stdout is used.
     * @param enabled
     */
    void setOutputToStdioEnabled(bool enabled = true); // stdout (or stderr)

    /**
     * @brief Enable or disable adding timestamp to each log message.
     * @param enabled
     */
    void setTimestampEnabled(bool enabled = true);

    /**
     * @brief Whether output to log file is enabled.
     * @return
     */
    bool isOutputToFileEnabled();

    /**
     * @brief Whether output to stdout/stderr is enabled.
     * @return
     */
    bool isOutputToStdioEnabled();

    /**
     * @brief Whether printing timestamp to log messages is enabled.
     * @return
     */
    bool isTimestampEnabled();

#ifndef _UNIT_TEST_
private:
#else
public:
#endif
    QString m_fileName;         ///< log file name
    QFile m_file;               ///< log file
    bool m_fileIsWritable;      ///< whether log file is writable
    bool m_outputToFileEnabled; ///< output to log file enabled
    bool m_outputToStdioEnabled; ///< output to stdout/stderr enabled
    bool m_timestampEnabled;    ///< adding timestamp to log message

signals:

public slots:
    /**
     * @brief Print log message. This method calls print(std::string).
     * @param message
     */
    void print(QString message);

    /**
     * @brief Print log message.
     * @param message
     */
    void print(std::string message);

    /**
     * @brief Print log message. This method calls print(std::string).
     * @param message
     */
    void print(const char* message);
};

#endif // LOGGER_H
