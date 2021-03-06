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

#ifndef MESSAGEUPDATE_H
#define MESSAGEUPDATE_H

#include <QDialog>
#include <queue>

namespace Ui {
class UpdateApplicationDialog;
}

/**
 * @brief Dialog to show availability and download new version of the application
 */
class UpdateApplicationDialog : public QDialog
{
    Q_OBJECT

public:
    UpdateApplicationDialog(QWidget *parent, QString version, std::queue<QString> message);
    ~UpdateApplicationDialog();

private slots:
    void on_commandLinkButton_clicked();

private:
    Ui::UpdateApplicationDialog *ui;
};

#endif // MESSAGEUPDATE_H
