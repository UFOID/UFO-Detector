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

#ifndef UPLOADER_H
#define UPLOADER_H

#include "config.h"
#include <QDialog>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QFile>

namespace Ui {
class VideoUploaderDialog;
}

/**
 * @brief Video uploader dialog class
 */
class VideoUploaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VideoUploaderDialog(QWidget *parent = 0, QString pathName = "", Config *configPtr = 0);
    ~VideoUploaderDialog();

private:
    Ui::VideoUploaderDialog *ui;
    QString filepath;
    Config *m_config;
    QNetworkAccessManager *manager;
    QFile* file;
    QString dateAndTime;
    QString successURL;
    QString errorURL;
    QString userToken;

private slots:
    void uploadFinish(QNetworkReply *r);
    void uploadError(QNetworkReply::NetworkError state);
    void on_buttonUpload_clicked();
    void progress(qint64 bytesSent, qint64 bytesTotal);
    void callAPI(QNetworkReply *reply);
    void errorCall(QNetworkReply *reply);
};

#endif // UPLOADER_H
