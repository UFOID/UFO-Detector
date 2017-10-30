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

#include "videouploaderdialog.h"
#include "ui_videouploaderdialog.h"
#include <QDebug>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QHttpMultiPart>


/* Note:
 * manager->finished() is not emited when the file is big (200mb).
 * See work around.
 */
VideoUploaderDialog::VideoUploaderDialog(QWidget *parent, QString pathName, Config *configPtr) :
    QDialog(parent), ui(new Ui::VideoUploaderDialog), filepath(pathName), m_config(configPtr)
{
    ui->setupUi(this);
    ui->lineFile->setText(pathName);
    manager = new QNetworkAccessManager(this);
    file = new QFile(filepath, this);
    ui->textBrowser->append("The link to your video will appear here after starting the upload");
    this->setWindowTitle("Upload video");
    this->setFixedSize(604,229);
    userToken = m_config->userTokenAtUfoId();

}

VideoUploaderDialog::~VideoUploaderDialog()
{
    qDebug() << "destroyed uploader";
    delete ui;
}

void VideoUploaderDialog::uploadFinish(QNetworkReply *r)
{
    Q_UNUSED(r);
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFinish(QNetworkReply*)));
    ui->progressBar->setValue(100);
    ui->textBrowser->setText("UPLOAD FINISHED");
    ui->textBrowser->append("Please go to your UFOID.net profile and click the edit icon on the video to add a title and description");
    ui->textBrowser->append("An admin will review and publish your video shortly");
}

void VideoUploaderDialog::uploadError(QNetworkReply::NetworkError state)
{
    qDebug() << "State" << state;
    ui->textBrowser->append("ERROR cannot upload video. Contact the developer if the error persists");
}

void VideoUploaderDialog::on_buttonUpload_clicked()
{
    ui->textBrowser->clear();
    dateAndTime = QDateTime::currentDateTime().toString("yyyy-MM-dd--hh-mm-ss");
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(callAPI(QNetworkReply*)) );


    QNetworkRequest req;
    QUrl url = QUrl("http://api.ufoid.net/upload/check");
    QUrlQuery query;
    query.addQueryItem("token", userToken);
    url.setQuery(query);

    req.setUrl(url);
    manager->post(req,QByteArray());
}

void VideoUploaderDialog::progress(qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << bytesSent << " from " << bytesTotal;
    if (bytesTotal!=0)
    {
        ui->progressBar->setValue(bytesSent*100/bytesTotal);
    }
    if (bytesSent==bytesTotal)        
    {
        ui->progressBar->setValue(95);
        ui->textBrowser->append("UPLOAD COMPLETE - Please hold on a few minutes until the video has been processed");
    }
}

void VideoUploaderDialog::callAPI(QNetworkReply* reply)
{
    qDebug() << "called API";

    if(reply->error() == QNetworkReply::NoError)
    {

        disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(callAPI(QNetworkReply*)) );

        QFileInfo fileInfo(file->fileName());

        QUrl url("http://api.ufoid.net/upload");
        QNetworkRequest req(url);

        QHttpMultiPart *multiPart = new QHttpMultiPart();

        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"token\""));
        textPart.setBody(userToken.toUtf8());

        QHttpPart imagePart;
        file->open(QIODevice::ReadOnly);
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                            QVariant("form-data; filename=\""+fileInfo.fileName()+"\"; name=\"file\""));
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(""));
        imagePart.setBodyDevice(file);
        file->setParent(multiPart);
        multiPart->append(imagePart);
        multiPart->append(textPart);

        QNetworkReply *reply = manager->post(req, multiPart);
        multiPart->setParent(reply);

        connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(uploadError(QNetworkReply::NetworkError)));
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFinish(QNetworkReply*)));

    }
    else
    {
        qDebug() << reply->errorString();
        ui->textBrowser->append("Invalid Upload token");
        ui->textBrowser->append("To upload a video create a free UFOID.net account and copy your given User Token from you User Profile in to the settings of the UFO Detector");
        ui->buttonUpload->setEnabled(false);
    }

    delete reply;
}

void VideoUploaderDialog::errorCall(QNetworkReply *reply)
{
    qDebug() << "error";
    delete reply;
}
