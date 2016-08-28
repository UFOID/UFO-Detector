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

#include "cameraresolutiondialog.h"
#include "ui_cameraresolutiondialog.h"

CameraResolutionDialog::CameraResolutionDialog(Camera *camera, Config *config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraResolutionDialog)
{
    ui->setupUi(this);
    m_camera = camera;
    connect(m_camera, SIGNAL(queryProgressChanged(int)), this, SLOT(onQueryProgressChanged(int)));
    m_config = config;

    ui->startQueryPushButton->setEnabled(m_camera->isInitialized());

    updateResolutionComboBox();
}

CameraResolutionDialog::~CameraResolutionDialog() {
    delete ui;
}

void CameraResolutionDialog::on_okPushButton_clicked() {
    emit resolutionAccepted(ui->resolutionComboBox->currentData().toSize());
    this->accept();
}

void CameraResolutionDialog::on_cancelPushButton_clicked() {
    this->reject();
}

void CameraResolutionDialog::on_startQueryPushButton_clicked() {
    qDebug() << "Starting query";
    ui->resolutionComboBox->setEnabled(true);
    ui->queryProgressBar->setEnabled(true);

    m_camera->queryAvailableResolutions();

    updateResolutionComboBox();
}

void CameraResolutionDialog::on_resolutionComboBox_currentIndexChanged(int index) {
    if ((index >= 0) && (index < ui->resolutionComboBox->count())) {
        ui->okPushButton->setEnabled(true);
    } else {
        ui->okPushButton->setEnabled(false);
    }
}

void CameraResolutionDialog::onQueryProgressChanged(int percent) {
    ui->queryProgressBar->setValue(percent);
}

void CameraResolutionDialog::updateResolutionComboBox() {
    QSize currentResolution(m_config->cameraWidth(), m_config->cameraHeight());
    QListIterator<QSize> resolutionListIt(m_camera->availableResolutions());

    while (resolutionListIt.hasNext()) {
        QSize resolution = resolutionListIt.next();
        QString itemStr = QString::number(resolution.width()) + " x " + QString::number(resolution.height());
        // add resolution to combo box if it's not already there
        int resolutionIndex = ui->resolutionComboBox->findText(itemStr);
        if (resolutionIndex < 0) {
            /// @todo check combo box items are sorted in ascending order
            ui->resolutionComboBox->addItem(itemStr, QVariant(resolution));
        }
    }
    ui->resolutionComboBox->setCurrentIndex(ui->resolutionComboBox->findData(currentResolution));
    if (ui->resolutionComboBox->count() > 0) {
        ui->resolutionComboBox->setEnabled(true);
    }
}
