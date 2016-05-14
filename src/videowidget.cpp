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
    m_dateTime = theDateTime;
    m_videoFileName = filepath + QString("/Capture--") + m_dateTime + QString(".avi");
    m_thumbnailFileName=filepath + QString("/thumbnails/") + m_dateTime + QString(".jpg");

    QLabel *thumbnailLabel = new QLabel(this);
    QLabel *lbl1 = new QLabel(m_dateTime, this);
    QLabel *lbl2 = new QLabel(videoLength, this);

    QImage thumbnail;
    thumbnail.load(m_thumbnailFileName);
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
    m_playButton = new ClickableLabel(this);
    m_playButton->setText("Play");
    hLayout2->addWidget(m_playButton, 22, Qt::AlignRight);

    m_uploadButton = new ClickableLabel(this);
    m_uploadButton->setText("Share");
    hLayout2->addWidget(m_uploadButton, 0, Qt::AlignRight);

    m_deleteButton = new ClickableLabel(this);
    m_deleteButton->setToolTip("Delete file from system and remove item from list");
    m_deleteButton->setText("Delete");
    hLayout2->addWidget(m_deleteButton, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(hLayout);
    mainLayout->addLayout(hLayout2);

    this->setLayout(mainLayout);
    this->resize(250,100);

}

VideoWidget::~VideoWidget()
{
    this->destroy(true,true);
    delete m_deleteButton;
    delete this->layout();
}

ClickableLabel *VideoWidget::deleteButton()
{
    return m_deleteButton;
}

QString VideoWidget::videoFileName()
{
    return m_videoFileName;
}

QString VideoWidget::thumbnailFileName()
{
    return m_thumbnailFileName;
}

QString VideoWidget::dateTime()
{
    return m_dateTime;
}

ClickableLabel *VideoWidget::uploadButton()
{
    return m_uploadButton;
}

ClickableLabel *VideoWidget::playButton()
{
    return m_playButton;
}

void VideoWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

