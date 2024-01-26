//
// Created by UFO ID on 16/12/2023.
//

#ifndef DETECTOR_SERVER_H
#define DETECTOR_SERVER_H


#include <QtCore>
#include "Camera.h"
#include "config.h"
#include "TimedMessage.cpp"
#include <QtHttpServer>

class Server : public QObject {
Q_OBJECT

public:

    Server(Camera *camera, Config *config);

private:
    Camera *m_camPtr;
    Config *m_config;
    QHttpServer httpServer;
    std::vector<TimedMessage> logs;

public slots:

    void log(const QString &message);
};


#endif //DETECTOR_SERVER_H
