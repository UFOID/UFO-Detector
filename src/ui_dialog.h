/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QPushButton *buttonTakePicture;
    QPushButton *buttonSave;
    QGraphicsView *image;
    QPushButton *buttonConnect;
    QLabel *labelInfo;
    QPushButton *buttonClear;
    QProgressBar *progressBar;
    QLabel *label;
    QLabel *label_2;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(830, 570);
        buttonTakePicture = new QPushButton(Dialog);
        buttonTakePicture->setObjectName(QStringLiteral("buttonTakePicture"));
        buttonTakePicture->setGeometry(QRect(50, 530, 75, 23));
        buttonSave = new QPushButton(Dialog);
        buttonSave->setObjectName(QStringLiteral("buttonSave"));
        buttonSave->setGeometry(QRect(690, 530, 75, 23));
        image = new QGraphicsView(Dialog);
        image->setObjectName(QStringLiteral("image"));
        image->setGeometry(QRect(50, 85, 721, 421));
        buttonConnect = new QPushButton(Dialog);
        buttonConnect->setObjectName(QStringLiteral("buttonConnect"));
        buttonConnect->setGeometry(QRect(140, 530, 75, 23));
        labelInfo = new QLabel(Dialog);
        labelInfo->setObjectName(QStringLiteral("labelInfo"));
        labelInfo->setGeometry(QRect(55, 60, 921, 16));
        QFont font;
        font.setPointSize(10);
        labelInfo->setFont(font);
        buttonClear = new QPushButton(Dialog);
        buttonClear->setObjectName(QStringLiteral("buttonClear"));
        buttonClear->setGeometry(QRect(230, 530, 75, 23));
        progressBar = new QProgressBar(Dialog);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(340, 530, 301, 23));
        progressBar->setValue(0);
        label = new QLabel(Dialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(50, 10, 371, 20));
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        label_2 = new QLabel(Dialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(50, 30, 371, 20));
        QFont font2;
        font2.setPointSize(8);
        font2.setBold(true);
        font2.setWeight(75);
        label_2->setFont(font2);

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0));
        buttonTakePicture->setText(QApplication::translate("Dialog", "Take picture", 0));
        buttonSave->setText(QApplication::translate("Dialog", "Save to file", 0));
        buttonConnect->setText(QApplication::translate("Dialog", "Connect", 0));
        labelInfo->setText(QApplication::translate("Dialog", "1. Take a picture with the webcam", 0));
        buttonClear->setText(QApplication::translate("Dialog", "Clear", 0));
        label->setText(QApplication::translate("Dialog", "Only select the sky in your area of detection", 0));
        label_2->setText(QApplication::translate("Dialog", "There should be no trees, telephone lines, etc. in your selection", 0));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
