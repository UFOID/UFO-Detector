//
// Created by UFO ID on 16/12/2023.
//

#include "Server.h"

Server::Server(Camera *camera, Config *config) : m_camPtr(camera), m_config(config) {


    const auto port = httpServer.listen(QHostAddress::Any, 8080);


    qDebug() << QCoreApplication::translate("QHttpServerExample",
                                            "Running on http://127.0.0.1:%1/ (Press CTRL+C to quit)").arg(port);


    httpServer.route("/preview", QHttpServerRequest::Method::Get,
                     [camera](const QHttpServerRequest &request) {
                         qDebug() << "/preview called";
                         cv::Mat frame = camera->getFrameDetector().frame;
                         std::vector<uchar> buffer;
                         cv::imencode(".jpg", frame, buffer);

                         // Convert the buffer to QByteArray
                         QByteArray byteArray(reinterpret_cast<const char *>(buffer.data()), buffer.size());

                         // Create the HTTP response with the image data
                         QHttpServerResponse response("image/jpeg", byteArray);
                         response.setHeader("Access-Control-Allow-Origin", "*");
                         return response;
                     });

    httpServer.route("/videos", QHttpServerRequest::Method::Get,
                     [config](const QHttpServerRequest &request) {
                         qDebug() << "/videos called";

                         auto videoDir = config->resultVideoDir();
                         QDir dir(videoDir);
                         QStringList filters;
                         filters << "*.mp4";
                         dir.setNameFilters(filters);
                         auto files = dir.entryInfoList();
                         QJsonArray videos;
                         for (const auto &file: files) {
                             if (file.fileName().contains("temp")) {
                                 continue;
                             }
                             QJsonObject video;
                             video["name"] = file.fileName();
                             video["path"] = file.filePath();
                             videos.append(video);
                         }
                         QJsonDocument responseDoc(videos);
                         QHttpServerResponse response("application/json", responseDoc.toJson());
                         response.setHeader("Access-Control-Allow-Origin", "*");
                         return response;
                     });

    // delete video path with filename as query parameter
    httpServer.route("/delete-video", QHttpServerRequest::Method::Get,
                     [config](const QHttpServerRequest &request) {
                         qDebug() << "/videos delete called";
                         auto query = request.url().query();
                         QUrlQuery urlQuery(query);
                         auto filename = urlQuery.queryItemValue("filename");
                         auto videoDir = config->resultVideoDir();
                         QDir dir(videoDir);
                         auto path = dir.filePath(filename);
                         QFile file(path);
                         if (file.exists()) {
                             file.remove();
                         } else {
                             qDebug() << "File does not exist";
                         }
                         QHttpServerResponse response("application/json", "ok");
                         response.setHeader("Access-Control-Allow-Origin", "*");
                         return response;
                     });

    // get logs
    httpServer.route("/logs", QHttpServerRequest::Method::Get,
                     [this](const QHttpServerRequest &request) {
                         qDebug() << "/logs called";
                         QJsonArray logsArray;
                         for (const auto &log: logs) {
                             QJsonObject logObject;
                             logObject["message"] = log.message;
                             logObject["timestamp"] = QString::number(log.timestamp);
                             logsArray.append(logObject);
                         }
                         QJsonDocument responseDoc(logsArray);
                         QHttpServerResponse response("application/json", responseDoc.toJson());
                         response.setHeader("Access-Control-Allow-Origin", "*");
                         return response;
                     });


    qDebug() << "Server created";
}

void Server::log(const QString &message) {
    logs.emplace_back(message);
}
