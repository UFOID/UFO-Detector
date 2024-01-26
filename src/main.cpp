#include <QCoreApplication>
#include <opencv2/core/core.hpp>
#include "Camera.h"
#include "datamanager.h"
#include "actualdetector.h"
#include "Server.h"

using namespace cv;

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    qInfo() << "Starting";

    qSetMessagePattern("[%{time hh:mm:ss} %{type}] - %{message}");

    std::unique_ptr<Config> config = std::make_unique<Config>();

    Camera myCam;
    bool cameraInit = myCam.startCamera();
    if (!cameraInit) {
        qInfo() << "Camera failed to start";
        return 0;
    }
    std::unique_ptr<DataManager> dataManager = std::make_unique<DataManager>(config.get());
    std::unique_ptr<ActualDetector> detector = std::make_unique<ActualDetector>(&myCam, config.get(),
                                                                                dataManager.get());

    Server server(&myCam, config.get());
    QObject::connect(detector.get(), SIGNAL(broadcastOutputText(QString)), &server, SLOT(log(QString)));

    bool init = detector->start();
    if (!init) {
        qInfo() << "Detector failed to start";
        return 1;
    }
    server.log("Detector started");

    qInfo() << "Finished main";
    return a.exec();
}
