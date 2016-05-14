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

#include "messageupdate.h"
#include "ui_messageupdate.h"
#include <QDesktopServices>

MessageUpdate::MessageUpdate(QWidget *parent, QString version, std::queue<QString> message) :
    QDialog(parent),
    ui(new Ui::MessageUpdate)
{
    ui->setupUi(this);
    ui->labelVersion->setText(tr("New version: %1").arg(version));
    while(message.size()>0)
	{
        ui->textBrowser->append(message.front());
        message.pop();
    }
}

void MessageUpdate::on_commandLinkButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://ufoid.net"));
}

MessageUpdate::~MessageUpdate()
{
    delete ui;
}

