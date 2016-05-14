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

#include "imageexplorer.h"
#include "ui_imageexplorer.h"
#include <QListWidget>
#include <QDir>
#include <QIcon>
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QDesktopServices>

ImageExplorer::ImageExplorer(QWidget *parent, Config *config) :
    QDialog(parent), ui(new Ui::ImageExplorer), m_config(config)
{
    ui->setupUi(this);

    mainDir = m_config->resultImageDir();
    if(m_config->saveResultImages())
	{
        ui->labelFolder->setText(mainDir);
    }
    else ui->labelFolder->setText("Activate the saving of images in the settings");

    QDir dir(mainDir);
    dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::LocaleAware);


    QStringList list = dir.entryList();
    for (int i = 0; i < list.size(); ++i)
	{
        if(list.at(i)!="." && list.at(i)!="..")
		{
            ui->listWidget->addItem(new QListWidgetItem(list.at(i)));
        }
    }

    connect(ui->listWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(displayFolder(QModelIndex)));
    manager = new QNetworkAccessManager(this);

    this->setWindowTitle("Upload Images");
    this->setFixedSize(611,631);

}

void ImageExplorer::displayFolder(QModelIndex index)
{
    disconnect(ui->listWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(displayFolder(QModelIndex)));

    folderName = ui->listWidget->item(index.row())->text();
    currentDir = mainDir+"/"+folderName+"/";
    ui->labelFolder->setText(currentDir);

    ui->listWidget->clear();
    ui->listWidget->setViewMode(QListWidget::IconMode);
    ui->listWidget->setResizeMode(QListWidget::Adjust);
    ui->listWidget->setIconSize(QSize(70,70));
    ui->listWidget->setSelectionMode(QListWidget::MultiSelection);
    ui->listWidget->setMovement(QListWidget::Static);

    QDir dir(currentDir);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::LocaleAware);
    dir.setNameFilters(QStringList()<<"*.jpg");

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
	{
        QFileInfo fileInfo = list.at(i);
        QIcon image(fileInfo.absoluteFilePath());
        ui->listWidget->addItem(new QListWidgetItem(image,fileInfo.fileName()));
    }

}

void ImageExplorer::uploadFinish(QNetworkReply* r)
{
    r->deleteLater();
    qDebug() << "finished uploading all images";
    ui->output->append(fileList.top() + "   uploaded");
    fileList.pop();

    if(!fileList.size())
	{
        ui->output->append("All images uploaded");
        ui->output->append("Thank you for helping to improve the algorithm");
    }
}


void ImageExplorer::uploadError(QNetworkReply::NetworkError state){
    qDebug() << "State" << state;
    ui->output->append("ERROR cannot upload image. Contact the developer if the error persists");
}

void ImageExplorer::openedPHP()
{
    qDebug()<< "called php";
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(openedPHP()));
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFinish(QNetworkReply*)));
    ui->output->append("Upload started");
    QList<QListWidgetItem*> list = ui->listWidget->selectedItems();

    for (int i = 0; i < list.size(); ++i)
	{
        QString fileToUpload = currentDir + list.at(i)->text();
        fileList.push(fileToUpload);
        QFile file(fileToUpload);
        QString folderToUpload = fileToUpload.remove(0,mainDir.size());
        QUrl url("ftp://ufoid.net/" + folderToUpload);
        url.setUserName("imagesaccount");
        url.setPassword("password");

        if (file.open(QIODevice::ReadOnly))
		{
            QByteArray data = file.readAll();
            QNetworkReply *reply =  manager->put(QNetworkRequest(url), data);
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(uploadError(QNetworkReply::NetworkError)));
        }
    }

}

void ImageExplorer::on_buttonBack_clicked()
{
    ui->labelFolder->setText(mainDir);
    ui->listWidget->clear();
    ui->listWidget->setViewMode(QListWidget::ListMode);
    ui->listWidget->setSelectionMode(QListWidget::SingleSelection);

    QDir dir(mainDir);
    dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::LocaleAware);

    QStringList list = dir.entryList();
    for (int i = 0; i < list.size(); ++i)
	{
        if(list.at(i)!="." && list.at(i)!="..")
		{
            ui->listWidget->addItem(new QListWidgetItem(list.at(i)));
        }
    }
    connect(ui->listWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(displayFolder(QModelIndex)));

}

void ImageExplorer::on_buttonUpload_clicked()
{
    if(ui->listWidget->selectedItems().size())
	{
        disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFinish(QNetworkReply*)));
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(openedPHP()) );
        QNetworkRequest req;
        req.setUrl(QUrl("http://ufoid.net/ximages/test.php?name="+folderName));
        req.setRawHeader( "User-Agent" , "Mozilla Firefox" );
        manager->get(req);
    }
    else ui->output->append("no images selected");

}

void ImageExplorer::on_commandLinkButton_clicked()
{
   QDesktopServices::openUrl(QUrl("http://ufoid.net/page/software"));
}

void ImageExplorer::on_buttonClear_clicked()
{
    ui->listWidget->clearSelection();
}

ImageExplorer::~ImageExplorer()
{
    qDebug() << "destructor imageexplorer";
    delete ui;
}
