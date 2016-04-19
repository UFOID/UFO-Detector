/********************************************************************************
** Form generated from reading UI file 'imageexplorer.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEEXPLORER_H
#define UI_IMAGEEXPLORER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_ImageExplorer
{
public:
    QListWidget *listWidget;
    QPushButton *buttonClear;
    QPushButton *buttonUpload;
    QPushButton *buttonBack;
    QLabel *labelFolder;
    QLabel *labelInfo;
    QTextBrowser *output;
    QCommandLinkButton *commandLinkButton;

    void setupUi(QDialog *ImageExplorer)
    {
        if (ImageExplorer->objectName().isEmpty())
            ImageExplorer->setObjectName(QStringLiteral("ImageExplorer"));
        ImageExplorer->resize(611, 631);
        listWidget = new QListWidget(ImageExplorer);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(10, 120, 591, 401));
        buttonClear = new QPushButton(ImageExplorer);
        buttonClear->setObjectName(QStringLiteral("buttonClear"));
        buttonClear->setGeometry(QRect(10, 530, 81, 23));
        buttonUpload = new QPushButton(ImageExplorer);
        buttonUpload->setObjectName(QStringLiteral("buttonUpload"));
        buttonUpload->setGeometry(QRect(100, 530, 91, 23));
        buttonBack = new QPushButton(ImageExplorer);
        buttonBack->setObjectName(QStringLiteral("buttonBack"));
        buttonBack->setGeometry(QRect(10, 90, 111, 23));
        labelFolder = new QLabel(ImageExplorer);
        labelFolder->setObjectName(QStringLiteral("labelFolder"));
        labelFolder->setGeometry(QRect(130, 90, 461, 21));
        QFont font;
        font.setPointSize(10);
        labelFolder->setFont(font);
        labelInfo = new QLabel(ImageExplorer);
        labelInfo->setObjectName(QStringLiteral("labelInfo"));
        labelInfo->setGeometry(QRect(20, 10, 601, 21));
        QFont font1;
        font1.setPointSize(12);
        labelInfo->setFont(font1);
        output = new QTextBrowser(ImageExplorer);
        output->setObjectName(QStringLiteral("output"));
        output->setGeometry(QRect(200, 530, 401, 81));
        commandLinkButton = new QCommandLinkButton(ImageExplorer);
        commandLinkButton->setObjectName(QStringLiteral("commandLinkButton"));
        commandLinkButton->setGeometry(QRect(20, 40, 291, 41));

        retranslateUi(ImageExplorer);

        QMetaObject::connectSlotsByName(ImageExplorer);
    } // setupUi

    void retranslateUi(QDialog *ImageExplorer)
    {
        ImageExplorer->setWindowTitle(QApplication::translate("ImageExplorer", "ImageExplorer", 0));
        buttonClear->setText(QApplication::translate("ImageExplorer", "Clear selection", 0));
        buttonUpload->setText(QApplication::translate("ImageExplorer", "Upload Images", 0));
        buttonBack->setText(QApplication::translate("ImageExplorer", "Back to main folder", 0));
        labelFolder->setText(QApplication::translate("ImageExplorer", "TextLabel", 0));
        labelInfo->setText(QApplication::translate("ImageExplorer", "Help improve the algorithm by uploading the images which show a known object", 0));
        commandLinkButton->setText(QApplication::translate("ImageExplorer", "Click here for an example of what to upload", 0));
    } // retranslateUi

};

namespace Ui {
    class ImageExplorer: public Ui_ImageExplorer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEEXPLORER_H
