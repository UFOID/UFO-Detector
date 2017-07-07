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
    void setFileName(QString fileName);
    QString fileName();
    void setOutputToFileEnabled(bool enabled = true);
    void setOutputToStdioEnabled(bool enabled = true); // stdout (or stderr)
    void setTimestampEnabled(bool enabled = true);
    bool isOutputToFileEnabled();
    bool isOutputToStdioEnabled();
    bool isTimestampEnabled();

#ifndef _UNIT_TEST_
private:
#endif
    QString m_fileName;
    QFile m_file;
    bool m_fileIsWritable;
    bool m_outputToFileEnabled;
    bool m_outputToStdioEnabled;
    bool m_timestampEnabled;

signals:

public slots:
    void print(QString message);
    void print(std::string message);
    void print(const char* message);
};

#endif // LOGGER_H
