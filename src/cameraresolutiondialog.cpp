#include "cameraresolutiondialog.h"
#include "ui_cameraresolutiondialog.h"

CameraResolutionDialog::CameraResolutionDialog(Camera *camera, Config *config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraResolutionDialog)
{
    ui->setupUi(this);
    m_camera = camera;
    m_config = config;
}

CameraResolutionDialog::~CameraResolutionDialog() {
    delete m_cameraInfo;
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
    // can't query busy camera so release it first
    m_camera->release();
    ui->resolutionComboBox->setEnabled(true);
    ui->queryProgressBar->setEnabled(true);
    m_cameraInfo = new CameraInfo(m_config->cameraIndex());
    connect(m_cameraInfo, SIGNAL(queryProgressChanged(int)), this, SLOT(onQueryProgressChanged(int)));
    m_cameraInfo->init();

    QSize currentResolution(m_config->cameraWidth(), m_config->cameraHeight());
    QListIterator<QSize> resolutionListIt(m_cameraInfo->availableResolutions());
    bool currentResolutionInList = false;
    while (resolutionListIt.hasNext()) {
        QSize resolution = resolutionListIt.next();
        QString itemStr = QString::number(resolution.width()) + " x " + QString::number(resolution.height());
        ui->resolutionComboBox->addItem(itemStr, QVariant(resolution));
        // select current resolution if it's found
        if (resolution == currentResolution) {
            currentResolutionInList = true;
            ui->resolutionComboBox->setCurrentIndex(ui->resolutionComboBox->count() - 1);
        }
    }
    if (!currentResolutionInList) {
        // show user nothing if non-matching resolution is given in settings
        ui->resolutionComboBox->setCurrentIndex(ui->resolutionComboBox->count());
    }
    qDebug() << "Reinitializing camera";
    m_camera->init();
    qDebug() << "Camera reinitialization done";
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
