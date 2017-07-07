#include "logger.h"

Logger::Logger(QObject *parent) : QObject(parent) {
    m_fileName = "";
    m_outputToFileEnabled = true;
    m_outputToStdioEnabled = true;
    m_timestampEnabled = true;
}

Logger::~Logger() {
    if (m_file.isOpen()) {
        m_file.flush();
        m_file.close();
    }
    std::cout.flush();
}

void Logger::setFileName(QString fileName) {
    m_fileName = fileName;
}

QString Logger::fileName() {
    return m_fileName;
}

void Logger::setOutputToFileEnabled(bool enabled) {
    m_outputToFileEnabled = enabled;
}

void Logger::setOutputToStdioEnabled(bool enabled) {
    m_outputToStdioEnabled = enabled;
}

void Logger::setTimestampEnabled(bool enabled) {
    m_timestampEnabled = enabled;
}

bool Logger::isOutputToFileEnabled() {
    return m_outputToFileEnabled;
}

bool Logger::isOutputToStdioEnabled() {
    return m_outputToStdioEnabled;
}

bool Logger::isTimestampEnabled() {
    return m_timestampEnabled;
}

void Logger::print(QString message) {
    if (!m_outputToFileEnabled && !m_outputToStdioEnabled) {
        return;
    }
    print(message.toStdString());
}

void Logger::print(std::string message) {
    if (!m_outputToFileEnabled && !m_outputToStdioEnabled) {
        return;
    }
    std::string outputStr;
    if (m_timestampEnabled) {
        QDateTime now = QDateTime::currentDateTime();
        QString timestamp = now.toString(LOGGER_TIMESTAMP_FORMAT_QT);
        outputStr = "[" + timestamp.toStdString() + "] ";
    }
    outputStr += message + '\n';
    if (m_outputToFileEnabled) {
        if (!m_file.isOpen()) {
            m_file.setFileName(m_fileName);
            m_file.open(QFile::WriteOnly | QFile::Append);
            if (m_file.isWritable()) {
                m_fileIsWritable = true;
            } else {
                m_fileIsWritable = false;
            }
        }
        if (m_fileIsWritable) {
            m_file.write(outputStr.data());
            m_file.flush();
        }
    }
    if (m_outputToStdioEnabled) {
        std::cout << outputStr << std::flush;
    }
}

void Logger::print(const char* message) {
    if (message && (m_outputToFileEnabled || m_outputToStdioEnabled)) {
        print(std::string(message));
    }
}

