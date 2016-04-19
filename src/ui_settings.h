/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QToolButton *toolButtonVideoPath;
    QLineEdit *lineH;
    QLineEdit *lineVideoPath;
    QPushButton *buttonXML;
    QLabel *labelW;
    QPushButton *buttonSave;
    QToolButton *toolButtonXMLfile;
    QLineEdit *lineW;
    QLineEdit *lineImagePath;
    QComboBox *comboBoxWebcam;
    QLabel *labelH;
    QCheckBox *checkBoxRectangle;
    QPushButton *buttonCancel;
    QLabel *labelImagePath;
    QLineEdit *lineXMLfile;
    QLabel *labelVideoPath;
    QCheckBox *checkBoxsaveImages;
    QLabel *labelWebcam;
    QLabel *labelXMLfile;
    QLabel *labelResolution;
    QToolButton *toolButtonImagePath;
    QLabel *label;
    QLineEdit *lineStatus;
    QLabel *labelMinPos;
    QLineEdit *lineMinPosRequired;
    QLabel *labelCodec;
    QComboBox *comboBoxCodec;
    QToolButton *toolButton;
    QLabel *label_2;
    QLineEdit *lineToken;
    QToolButton *toolButtonToken;

    void setupUi(QDialog *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName(QStringLiteral("Settings"));
        Settings->resize(389, 424);
        toolButtonVideoPath = new QToolButton(Settings);
        toolButtonVideoPath->setObjectName(QStringLiteral("toolButtonVideoPath"));
        toolButtonVideoPath->setGeometry(QRect(330, 118, 25, 19));
        lineH = new QLineEdit(Settings);
        lineH->setObjectName(QStringLiteral("lineH"));
        lineH->setGeometry(QRect(270, 55, 51, 20));
        lineVideoPath = new QLineEdit(Settings);
        lineVideoPath->setObjectName(QStringLiteral("lineVideoPath"));
        lineVideoPath->setGeometry(QRect(130, 117, 191, 20));
        lineVideoPath->setReadOnly(true);
        buttonXML = new QPushButton(Settings);
        buttonXML->setObjectName(QStringLiteral("buttonXML"));
        buttonXML->setGeometry(QRect(190, 202, 131, 24));
        labelW = new QLabel(Settings);
        labelW->setObjectName(QStringLiteral("labelW"));
        labelW->setGeometry(QRect(130, 60, 31, 16));
        buttonSave = new QPushButton(Settings);
        buttonSave->setObjectName(QStringLiteral("buttonSave"));
        buttonSave->setGeometry(QRect(95, 380, 75, 26));
        toolButtonXMLfile = new QToolButton(Settings);
        toolButtonXMLfile->setObjectName(QStringLiteral("toolButtonXMLfile"));
        toolButtonXMLfile->setGeometry(QRect(330, 146, 25, 19));
        lineW = new QLineEdit(Settings);
        lineW->setObjectName(QStringLiteral("lineW"));
        lineW->setGeometry(QRect(168, 55, 51, 20));
        lineImagePath = new QLineEdit(Settings);
        lineImagePath->setObjectName(QStringLiteral("lineImagePath"));
        lineImagePath->setEnabled(true);
        lineImagePath->setGeometry(QRect(130, 270, 191, 20));
        lineImagePath->setStyleSheet(QStringLiteral(""));
        lineImagePath->setReadOnly(true);
        comboBoxWebcam = new QComboBox(Settings);
        comboBoxWebcam->setObjectName(QStringLiteral("comboBoxWebcam"));
        comboBoxWebcam->setGeometry(QRect(130, 20, 191, 22));
        labelH = new QLabel(Settings);
        labelH->setObjectName(QStringLiteral("labelH"));
        labelH->setGeometry(QRect(230, 60, 41, 16));
        checkBoxRectangle = new QCheckBox(Settings);
        checkBoxRectangle->setObjectName(QStringLiteral("checkBoxRectangle"));
        checkBoxRectangle->setGeometry(QRect(30, 230, 241, 17));
        buttonCancel = new QPushButton(Settings);
        buttonCancel->setObjectName(QStringLiteral("buttonCancel"));
        buttonCancel->setGeometry(QRect(190, 380, 75, 26));
        labelImagePath = new QLabel(Settings);
        labelImagePath->setObjectName(QStringLiteral("labelImagePath"));
        labelImagePath->setEnabled(true);
        labelImagePath->setGeometry(QRect(30, 276, 86, 13));
        lineXMLfile = new QLineEdit(Settings);
        lineXMLfile->setObjectName(QStringLiteral("lineXMLfile"));
        lineXMLfile->setGeometry(QRect(130, 146, 191, 20));
        lineXMLfile->setReadOnly(true);
        labelVideoPath = new QLabel(Settings);
        labelVideoPath->setObjectName(QStringLiteral("labelVideoPath"));
        labelVideoPath->setGeometry(QRect(30, 122, 86, 13));
        checkBoxsaveImages = new QCheckBox(Settings);
        checkBoxsaveImages->setObjectName(QStringLiteral("checkBoxsaveImages"));
        checkBoxsaveImages->setEnabled(true);
        checkBoxsaveImages->setGeometry(QRect(30, 251, 86, 17));
        checkBoxsaveImages->setCheckable(true);
        checkBoxsaveImages->setChecked(false);
        labelWebcam = new QLabel(Settings);
        labelWebcam->setObjectName(QStringLiteral("labelWebcam"));
        labelWebcam->setGeometry(QRect(30, 25, 91, 16));
        labelXMLfile = new QLabel(Settings);
        labelXMLfile->setObjectName(QStringLiteral("labelXMLfile"));
        labelXMLfile->setGeometry(QRect(30, 148, 51, 16));
        labelResolution = new QLabel(Settings);
        labelResolution->setObjectName(QStringLiteral("labelResolution"));
        labelResolution->setGeometry(QRect(30, 60, 91, 16));
        toolButtonImagePath = new QToolButton(Settings);
        toolButtonImagePath->setObjectName(QStringLiteral("toolButtonImagePath"));
        toolButtonImagePath->setEnabled(true);
        toolButtonImagePath->setGeometry(QRect(330, 270, 25, 19));
        label = new QLabel(Settings);
        label->setObjectName(QStringLiteral("label"));
        label->setEnabled(false);
        label->setGeometry(QRect(30, 179, 101, 16));
        lineStatus = new QLineEdit(Settings);
        lineStatus->setObjectName(QStringLiteral("lineStatus"));
        lineStatus->setEnabled(false);
        lineStatus->setGeometry(QRect(130, 177, 191, 20));
        lineStatus->setReadOnly(true);
        labelMinPos = new QLabel(Settings);
        labelMinPos->setObjectName(QStringLiteral("labelMinPos"));
        labelMinPos->setGeometry(QRect(30, 303, 241, 16));
        lineMinPosRequired = new QLineEdit(Settings);
        lineMinPosRequired->setObjectName(QStringLiteral("lineMinPosRequired"));
        lineMinPosRequired->setGeometry(QRect(280, 300, 41, 20));
        labelCodec = new QLabel(Settings);
        labelCodec->setObjectName(QStringLiteral("labelCodec"));
        labelCodec->setGeometry(QRect(30, 90, 86, 13));
        comboBoxCodec = new QComboBox(Settings);
        comboBoxCodec->setObjectName(QStringLiteral("comboBoxCodec"));
        comboBoxCodec->setGeometry(QRect(130, 83, 191, 22));
        toolButton = new QToolButton(Settings);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        toolButton->setGeometry(QRect(330, 85, 25, 19));
        label_2 = new QLabel(Settings);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 330, 131, 16));
        lineToken = new QLineEdit(Settings);
        lineToken->setObjectName(QStringLiteral("lineToken"));
        lineToken->setGeometry(QRect(130, 326, 191, 20));
        toolButtonToken = new QToolButton(Settings);
        toolButtonToken->setObjectName(QStringLiteral("toolButtonToken"));
        toolButtonToken->setGeometry(QRect(330, 326, 25, 19));

        retranslateUi(Settings);

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QDialog *Settings)
    {
        Settings->setWindowTitle(QApplication::translate("Settings", "Dialog", 0));
        toolButtonVideoPath->setText(QApplication::translate("Settings", "...", 0));
        buttonXML->setText(QApplication::translate("Settings", "Select detection area", 0));
        labelW->setText(QApplication::translate("Settings", "Width:", 0));
        buttonSave->setText(QApplication::translate("Settings", "Save", 0));
        toolButtonXMLfile->setText(QApplication::translate("Settings", "...", 0));
        comboBoxWebcam->clear();
        comboBoxWebcam->insertItems(0, QStringList()
         << QApplication::translate("Settings", "Webcam 1", 0)
         << QApplication::translate("Settings", "Webcam 2", 0)
         << QApplication::translate("Settings", "Webcam 3", 0)
         << QApplication::translate("Settings", "Webcam 4", 0)
        );
        labelH->setText(QApplication::translate("Settings", "Height:", 0));
        checkBoxRectangle->setText(QApplication::translate("Settings", "Draw rectangle around object in the video", 0));
        buttonCancel->setText(QApplication::translate("Settings", "Close", 0));
        labelImagePath->setText(QApplication::translate("Settings", "Image Path:", 0));
        lineXMLfile->setText(QString());
        labelVideoPath->setText(QApplication::translate("Settings", "Video Path:", 0));
        checkBoxsaveImages->setText(QApplication::translate("Settings", "Save images", 0));
        labelWebcam->setText(QApplication::translate("Settings", "Select Webcam:", 0));
        labelXMLfile->setText(QApplication::translate("Settings", "Area File:", 0));
        labelResolution->setText(QApplication::translate("Settings", "Select Resolution:", 0));
        toolButtonImagePath->setText(QApplication::translate("Settings", "...", 0));
        label->setText(QApplication::translate("Settings", "Area Status:", 0));
        lineStatus->setText(QString());
        labelMinPos->setText(QApplication::translate("Settings", "Minimum required number of positive detections:", 0));
        labelCodec->setText(QApplication::translate("Settings", "Video Codec:", 0));
        comboBoxCodec->clear();
        comboBoxCodec->insertItems(0, QStringList()
         << QApplication::translate("Settings", "Raw Video", 0)
         << QApplication::translate("Settings", "FFV1 Lossless Video", 0)
         << QApplication::translate("Settings", "Lagarith Lossless Video", 0)
        );
        toolButton->setText(QApplication::translate("Settings", "?", 0));
        label_2->setText(QApplication::translate("Settings", "UFOID User Token:", 0));
        toolButtonToken->setText(QApplication::translate("Settings", "?", 0));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
