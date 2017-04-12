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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_config = new Config();
    m_camera = new Camera(m_config->cameraIndex(), m_config->cameraWidth(), m_config->cameraHeight());
    m_camera->init();
    m_editor = new DetectionAreaEditDialog(this, m_camera, m_config);
    qDebug() << "Application dir path:" << QApplication::applicationDirPath();
    on_openDetectionAreaEditorButton_clicked();
}

MainWindow::~MainWindow() {
    delete ui;
    m_camera->deleteLater();
    m_config->deleteLater();
    m_editor->deleteLater();
}

void MainWindow::on_openDetectionAreaEditorButton_clicked() {
    if (!m_editor->isVisible()) {
        m_editor->setModal(true);
        m_editor->setAttribute(Qt::WA_AlwaysStackOnTop);
        m_editor->show();
    }
}
