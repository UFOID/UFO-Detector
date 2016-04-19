/********************************************************************************
** Form generated from reading UI file 'uploader.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UPLOADER_H
#define UI_UPLOADER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_Uploader
{
public:
    QLabel *label;
    QLineEdit *lineFile;
    QPushButton *buttonUpload;
    QProgressBar *progressBar;
    QTextBrowser *textBrowser;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;

    void setupUi(QDialog *Uploader)
    {
        if (Uploader->objectName().isEmpty())
            Uploader->setObjectName(QStringLiteral("Uploader"));
        Uploader->resize(604, 229);
        label = new QLabel(Uploader);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 70, 71, 16));
        lineFile = new QLineEdit(Uploader);
        lineFile->setObjectName(QStringLiteral("lineFile"));
        lineFile->setGeometry(QRect(110, 70, 451, 20));
        lineFile->setReadOnly(true);
        buttonUpload = new QPushButton(Uploader);
        buttonUpload->setObjectName(QStringLiteral("buttonUpload"));
        buttonUpload->setGeometry(QRect(20, 180, 75, 23));
        progressBar = new QProgressBar(Uploader);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setEnabled(true);
        progressBar->setGeometry(QRect(110, 180, 491, 23));
        progressBar->setValue(0);
        textBrowser = new QTextBrowser(Uploader);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(110, 100, 451, 51));
        textBrowser->setReadOnly(false);
        label_2 = new QLabel(Uploader);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 100, 71, 16));
        label_3 = new QLabel(Uploader);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 10, 301, 21));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);
        label_4 = new QLabel(Uploader);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 35, 571, 16));

        retranslateUi(Uploader);

        QMetaObject::connectSlotsByName(Uploader);
    } // setupUi

    void retranslateUi(QDialog *Uploader)
    {
        Uploader->setWindowTitle(QApplication::translate("Uploader", "Dialog", 0));
        label->setText(QApplication::translate("Uploader", "Selected file:", 0));
        buttonUpload->setText(QApplication::translate("Uploader", "Upload", 0));
        label_2->setText(QApplication::translate("Uploader", "Output:", 0));
        label_3->setText(QApplication::translate("Uploader", "Upload video to UFOID.net", 0));
        label_4->setText(QApplication::translate("Uploader", "Submit your video and have it analyzed by other users", 0));
    } // retranslateUi

};

namespace Ui {
    class Uploader: public Ui_Uploader {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UPLOADER_H
