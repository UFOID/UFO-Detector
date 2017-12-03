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

#include "updateapplicationdialog.h"
#include "ui_updateapplicationdialog.h"
#include <QDesktopServices>

UpdateApplicationDialog::UpdateApplicationDialog(QWidget *parent, QString version, std::queue<QString> message) :
    QDialog(parent),
    ui(new Ui::UpdateApplicationDialog)
{
    ui->setupUi(this);
    ui->labelVersion->setText(tr("New version: %1").arg(version));
    while(message.size()>0)
	{
        ui->textBrowser->append(message.front());
        message.pop();
    }
}

void UpdateApplicationDialog::on_commandLinkButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.theblackvault.com/casefiles/the-ufo-detector/"));
}

UpdateApplicationDialog::~UpdateApplicationDialog()
{
    delete ui;
}

