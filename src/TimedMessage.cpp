//
// Created by UFO ID on 26/12/2023.
//

#include <QString>
#include <chrono>

struct TimedMessage {
    QString message;
    long timestamp;

    TimedMessage(const QString &message)
            : message(message), timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()) {}
};