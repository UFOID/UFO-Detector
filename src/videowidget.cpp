/**
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

#include "videowidget.h"
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QHBoxLayout>
#include <QPainter>
#include <iostream>
#include <QMouseEvent>
#include <clickablelabel.h>
#include "mainwindow.h"
#include <QDebug>

using namespace std;


VideoWidget::VideoWidget(QWidget *parent, QString filepath, QString theDateTime, QString videoLength) :
    QWidget(parent)
{

    myParent = qobject_cast<MainWindow*>(parent);
    /// @todo take full file path from result data file
    fullFilePath = filepath+QString("/Capture--")+theDateTime+QString(".avi");
    dateTime = theDateTime;
    /// @todo take full thumbnail path from result data file
    thumbnailPath=filepath+QString("/thumbnails/")+theDateTime+QString(".jpg");

    QLabel *thumbnailLabel = new QLabel(this);
    QLabel *lbl1 = new QLabel(dateTime, this);
    QLabel *lbl2 = new QLabel(videoLength, this);

    QImage thumbnail;
    thumbnail.load(thumbnailPath);
    thumbnailLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    thumbnailLabel->setPixmap(QPixmap::fromImage(thumbnail));

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(lbl1);
    vLayout->addWidget(lbl2);
    vLayout->addStretch(3);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(thumbnailLabel);
    hLayout->addLayout(vLayout);

    QHBoxLayout *hLayout2 = new QHBoxLayout;
    smallPlay = new ClickableLabel(this);
    smallPlay->setText("Play");
    hLayout2->addWidget(smallPlay, 22, Qt::AlignRight);

    smallUpload = new ClickableLabel(this);
    smallUpload->setText("Share");
    hLayout2->addWidget(smallUpload, 0, Qt::AlignRight);

    smallRed = new ClickableLabel(this);
    smallRed->setToolTip("Delete file from system and remove item from list");
    smallRed->setText("Delete");
    hLayout2->addWidget(smallRed, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(hLayout);
    mainLayout->addLayout(hLayout2);

    this->setStyleSheet("background-color:#3C4A62; border: 1px solid #515C65;color: white;");
    this->setLayout(mainLayout);
    this->resize(250,100);

}

VideoWidget::~VideoWidget()
{
    this->destroy(true,true);
    delete smallRed;
    delete this->layout();
}

ClickableLabel *VideoWidget::getClickableLabel()
{
    return smallRed;
}

QString VideoWidget::getPathname()
{
    return fullFilePath;
}

QString VideoWidget::getDateTime()
{
    return dateTime;
}

ClickableLabel *VideoWidget::getUploadLabel()
{
    return smallUpload;
}

ClickableLabel *VideoWidget::getPlayLabel()
{
    return smallPlay;
}

void VideoWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
