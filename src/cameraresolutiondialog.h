#ifndef CAMERARESOLUTIONDIALOG_H
#define CAMERARESOLUTIONDIALOG_H

#include "config.h"
#include "camera.h"
#include "camerainfo.h"
#include <QDialog>

namespace Ui {
class CameraResolutionDialog;
}

/**
 * @brief The CameraResolutionDialog class
 * Dialog for querying supported resolutions from camera
 *
 * @todo use cache so that don't need to query again when dialog is reopened; need to check camera is the same
 */
class CameraResolutionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CameraResolutionDialog(Camera* camera, Config* config, QWidget *parent = 0);
    ~CameraResolutionDialog();

signals:
    /**
     * @brief resolutionAccepted emitted when user has selected and accepted a resolution and the dialog is closing
     * @param resolution camera resolution
     */
    void resolutionAccepted(QSize resolution);

public slots:
    void on_okPushButton_clicked();
    void on_cancelPushButton_clicked();
    void on_startQueryPushButton_clicked();
    void on_resolutionComboBox_currentIndexChanged(int index);
    void onQueryProgressChanged(int percent);

private:
    Ui::CameraResolutionDialog *ui;
    Config* m_config;
    Camera* m_camera;
    CameraInfo* m_cameraInfo;
};

#endif // CAMERARESOLUTIONDIALOG_H
