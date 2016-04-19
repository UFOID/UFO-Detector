/********************************************************************************
** Form generated from reading UI file 'messageupdate.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEUPDATE_H
#define UI_MESSAGEUPDATE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_MessageUpdate
{
public:
    QLabel *labelFixed;
    QLabel *labelVersion;
    QTextBrowser *textBrowser;
    QCommandLinkButton *commandLinkButton;

    void setupUi(QDialog *MessageUpdate)
    {
        if (MessageUpdate->objectName().isEmpty())
            MessageUpdate->setObjectName(QStringLiteral("MessageUpdate"));
        MessageUpdate->resize(400, 350);
        labelFixed = new QLabel(MessageUpdate);
        labelFixed->setObjectName(QStringLiteral("labelFixed"));
        labelFixed->setGeometry(QRect(30, 10, 261, 31));
        QFont font;
        font.setPointSize(12);
        labelFixed->setFont(font);
        labelVersion = new QLabel(MessageUpdate);
        labelVersion->setObjectName(QStringLiteral("labelVersion"));
        labelVersion->setGeometry(QRect(30, 40, 261, 16));
        labelVersion->setFont(font);
        textBrowser = new QTextBrowser(MessageUpdate);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(30, 70, 331, 211));
        commandLinkButton = new QCommandLinkButton(MessageUpdate);
        commandLinkButton->setObjectName(QStringLiteral("commandLinkButton"));
        commandLinkButton->setGeometry(QRect(30, 290, 331, 41));

        retranslateUi(MessageUpdate);

        QMetaObject::connectSlotsByName(MessageUpdate);
    } // setupUi

    void retranslateUi(QDialog *MessageUpdate)
    {
        MessageUpdate->setWindowTitle(QApplication::translate("MessageUpdate", "Dialog", 0));
        labelFixed->setText(QApplication::translate("MessageUpdate", "New UFO Detector update available!", 0));
        labelVersion->setText(QApplication::translate("MessageUpdate", "Version:", 0));
        commandLinkButton->setText(QApplication::translate("MessageUpdate", "Download here", 0));
    } // retranslateUi

};

namespace Ui {
    class MessageUpdate: public Ui_MessageUpdate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEUPDATE_H
