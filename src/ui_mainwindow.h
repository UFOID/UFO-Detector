/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QCheckBox *checkBox;
    QPushButton *startButton;
    QTextBrowser *outputText;
    QLabel *statusLabel;
    QLabel *webcam;
    QPushButton *stopButton;
    QPushButton *buttonClear;
    QPushButton *settingsButton;
    QLabel *label_8;
    QLineEdit *lineNoise;
    QSlider *sliderNoise;
    QLabel *label_9;
    QLineEdit *lineCount;
    QLabel *label_2;
    QListWidget *myList;
    QPushButton *recordingTestButton;
    QPushButton *aboutButton;
    QPushButton *buttonImageExpl;
    QProgressBar *progressBar;
    QToolButton *toolButtonNoise;
    QSlider *sliderThresh;
    QLineEdit *lineThresh;
    QToolButton *toolButtonThresh;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_3;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1060, 630);
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(240, 240, 240, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(247, 247, 247, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(120, 120, 120, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(160, 160, 160, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush3);
        QBrush brush6(QColor(255, 255, 220, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        MainWindow->setPalette(palette);
        MainWindow->setAutoFillBackground(false);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        checkBox = new QCheckBox(centralwidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(395, 430, 421, 17));
        startButton = new QPushButton(centralwidget);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setGeometry(QRect(945, 490, 81, 24));
        outputText = new QTextBrowser(centralwidget);
        outputText->setObjectName(QStringLiteral("outputText"));
        outputText->setGeometry(QRect(395, 450, 515, 135));
        outputText->setStyleSheet(QStringLiteral(""));
        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));
        statusLabel->setEnabled(true);
        statusLabel->setGeometry(QRect(270, 385, 381, 26));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        statusLabel->setFont(font);
        webcam = new QLabel(centralwidget);
        webcam->setObjectName(QStringLiteral("webcam"));
        webcam->setGeometry(QRect(270, 20, 640, 360));
        QFont font1;
        font1.setPointSize(14);
        webcam->setFont(font1);
        webcam->setAutoFillBackground(true);
        stopButton = new QPushButton(centralwidget);
        stopButton->setObjectName(QStringLiteral("stopButton"));
        stopButton->setGeometry(QRect(945, 525, 81, 24));
        buttonClear = new QPushButton(centralwidget);
        buttonClear->setObjectName(QStringLiteral("buttonClear"));
        buttonClear->setGeometry(QRect(282, 490, 86, 24));
        settingsButton = new QPushButton(centralwidget);
        settingsButton->setObjectName(QStringLiteral("settingsButton"));
        settingsButton->setGeometry(QRect(940, 55, 75, 23));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(960, 200, 56, 13));
        QFont font2;
        font2.setPointSize(7);
        label_8->setFont(font2);
        lineNoise = new QLineEdit(centralwidget);
        lineNoise->setObjectName(QStringLiteral("lineNoise"));
        lineNoise->setGeometry(QRect(936, 190, 20, 20));
        lineNoise->setStyleSheet(QStringLiteral(""));
        lineNoise->setReadOnly(true);
        sliderNoise = new QSlider(centralwidget);
        sliderNoise->setObjectName(QStringLiteral("sliderNoise"));
        sliderNoise->setGeometry(QRect(945, 98, 61, 81));
        sliderNoise->setMinimum(1);
        sliderNoise->setMaximum(5);
        sliderNoise->setPageStep(5);
        sliderNoise->setSliderPosition(1);
        sliderNoise->setOrientation(Qt::Vertical);
        sliderNoise->setTickPosition(QSlider::TicksBothSides);
        sliderNoise->setTickInterval(1);
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(960, 190, 51, 13));
        label_9->setFont(font2);
        lineCount = new QLineEdit(centralwidget);
        lineCount->setObjectName(QStringLiteral("lineCount"));
        lineCount->setGeometry(QRect(936, 360, 21, 20));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(960, 355, 71, 21));
        label_2->setFont(font2);
        myList = new QListWidget(centralwidget);
        myList->setObjectName(QStringLiteral("myList"));
        myList->setGeometry(QRect(15, 20, 240, 566));
        recordingTestButton = new QPushButton(centralwidget);
        recordingTestButton->setObjectName(QStringLiteral("recordingTestButton"));
        recordingTestButton->setEnabled(true);
        recordingTestButton->setGeometry(QRect(275, 430, 86, 24));
        aboutButton = new QPushButton(centralwidget);
        aboutButton->setObjectName(QStringLiteral("aboutButton"));
        aboutButton->setGeometry(QRect(940, 20, 75, 23));
        buttonImageExpl = new QPushButton(centralwidget);
        buttonImageExpl->setObjectName(QStringLiteral("buttonImageExpl"));
        buttonImageExpl->setGeometry(QRect(282, 525, 86, 24));
        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setEnabled(true);
        progressBar->setGeometry(QRect(580, 385, 331, 23));
        progressBar->setValue(0);
        toolButtonNoise = new QToolButton(centralwidget);
        toolButtonNoise->setObjectName(QStringLiteral("toolButtonNoise"));
        toolButtonNoise->setGeometry(QRect(1020, 190, 25, 19));
        sliderThresh = new QSlider(centralwidget);
        sliderThresh->setObjectName(QStringLiteral("sliderThresh"));
        sliderThresh->setGeometry(QRect(945, 230, 61, 81));
        sliderThresh->setMinimum(10);
        sliderThresh->setMaximum(20);
        sliderThresh->setSingleStep(5);
        sliderThresh->setPageStep(5);
        sliderThresh->setSliderPosition(10);
        sliderThresh->setOrientation(Qt::Vertical);
        sliderThresh->setTickPosition(QSlider::TicksBothSides);
        sliderThresh->setTickInterval(1);
        lineThresh = new QLineEdit(centralwidget);
        lineThresh->setObjectName(QStringLiteral("lineThresh"));
        lineThresh->setGeometry(QRect(935, 320, 20, 20));
        lineThresh->setStyleSheet(QStringLiteral(""));
        lineThresh->setReadOnly(true);
        toolButtonThresh = new QToolButton(centralwidget);
        toolButtonThresh->setObjectName(QStringLiteral("toolButtonThresh"));
        toolButtonThresh->setGeometry(QRect(1020, 320, 25, 19));
        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(960, 330, 41, 16));
        label_10->setFont(font2);
        label_11 = new QLabel(centralwidget);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(960, 320, 51, 13));
        label_11->setFont(font2);
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(960, 370, 71, 10));
        label_3->setFont(font2);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1060, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
#ifndef QT_NO_TOOLTIP
        checkBox->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Deactivate for better performance</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
        checkBox->setText(QApplication::translate("MainWindow", "Display webcam during detection (not recommended)", 0));
        startButton->setText(QApplication::translate("MainWindow", "Detect", 0));
        statusLabel->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" color:#ff0004;\">Detection not running</span></p></body></html>", 0));
        webcam->setText(QString());
        stopButton->setText(QApplication::translate("MainWindow", "Stop", 0));
        buttonClear->setText(QApplication::translate("MainWindow", "Clear Messages", 0));
        settingsButton->setText(QApplication::translate("MainWindow", "Settings", 0));
        label_8->setText(QApplication::translate("MainWindow", "by pixel size", 0));
        lineNoise->setText(QApplication::translate("MainWindow", "1", 0));
        label_9->setText(QApplication::translate("MainWindow", "Filter noise", 0));
        lineCount->setText(QApplication::translate("MainWindow", "0", 0));
        label_2->setText(QApplication::translate("MainWindow", "Number of", 0));
        recordingTestButton->setText(QApplication::translate("MainWindow", "PushButton", 0));
        aboutButton->setText(QApplication::translate("MainWindow", "About", 0));
        buttonImageExpl->setText(QApplication::translate("MainWindow", "Upload Images", 0));
        toolButtonNoise->setText(QApplication::translate("MainWindow", "?", 0));
        lineThresh->setText(QApplication::translate("MainWindow", "10", 0));
        toolButtonThresh->setText(QApplication::translate("MainWindow", "?", 0));
        label_10->setText(QApplication::translate("MainWindow", "threhold", 0));
        label_11->setText(QApplication::translate("MainWindow", "Select ", 0));
        label_3->setText(QApplication::translate("MainWindow", "recorded videos", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
