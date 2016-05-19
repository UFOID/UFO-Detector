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
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(successReply(QNetworkReply*)));
    QNetworkRequest req;
    req.setUrl(QUrl(successURL));
    manager->get(req);
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
    req.setUrl(QUrl("http://ufoid.net/api/geturl/"+userToken));

    manager->get(req);
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
        manager = new QNetworkAccessManager();
        uploadFinish(NULL);
    }
}

void VideoUploaderDialog::callAPI(QNetworkReply* reply)
{
    qDebug() << "called API";

    if(reply->error() == QNetworkReply::NoError)
    {
        QString strReply = (QString)reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject jsonObject = jsonResponse.object().value("params").toObject().value("ftp").toObject();
        QString videoID = jsonResponse.object().value("params").toObject().value("videoId").toString();
        successURL = jsonResponse.object().value("params").toObject().value("success").toString();
        errorURL = jsonResponse.object().value("params").toObject().value("error").toString();

        QFileInfo fileInfo(file->fileName());
        QString filename(fileInfo.fileName());
        disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(callAPI(QNetworkReply*)) );
        //connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFinish(QNetworkReply*)));
        qDebug() << successURL;
        QString host = jsonObject["host"].toString();
        QString user = jsonObject["user"].toString();
        QString pass = jsonObject["pass"].toString();
        QString path = "ftp://"+host+"/"+videoID+"--"+filename;
        QString videoURL = jsonResponse.object().value("params").toObject().value("url").toString();
        QUrl url(path);
        url.setUserName(user);
        url.setPassword(pass);
        if (file->open(QFile::ReadOnly))
        {
            QNetworkReply *reply =  manager->put(QNetworkRequest(url),file);
            connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(uploadError(QNetworkReply::NetworkError)));
            ui->textBrowser->setText("Upload started. Do not close this window.");
            ui->textBrowser->append("Edit the information about your video at "+ videoURL);
        }

    }
    else
    {
        ui->textBrowser->setText("Invalid User Token");
        ui->textBrowser->setText("To upload a video create a free UFOID.net account and copy your given User Token in to the settings of the UFO Detector");
        ui->buttonUpload->setEnabled(false);
    }

    delete reply;
}

void VideoUploaderDialog::successReply(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        QString strReply = (QString)reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        qDebug() << jsonResponse;
        QJsonObject object = jsonResponse.object();
        bool isSuccess = false;
        if (object.value("success").isBool())
        {
            isSuccess = object.value("success").toBool();
        }
        if (isSuccess)
        {
            ui->textBrowser->append("UPLOAD FINISHED");
            ui->textBrowser->append("Don't forget to add a title and description to the video via the link above");
            ui->textBrowser->append("An admin will review and publish your video shortly");
        }
        else
        {
            ui->textBrowser->append("UPLOAD ERROR");
            ui->textBrowser->append("Please try again or contact contact@ufoid.net if the error persists");
            disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(successReply(QNetworkReply*)));
            connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(errorCall(QNetworkReply*)));
            QNetworkRequest req;
            req.setUrl(QUrl(errorURL));
            manager->get(req);
        }

    }
}

void VideoUploaderDialog::errorCall(QNetworkReply *reply)
{
    qDebug() << "error";
    delete reply;
}
