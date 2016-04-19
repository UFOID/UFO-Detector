/**
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

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

class MainWindow;
class ClickableLabel;

namespace Ui {
class VideoWidget;
}

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = 0, QString filepath = "", QString theDateTime = "", QString videoLength="");
    ~VideoWidget();
    ClickableLabel* getClickableLabel();
    QString getPathname();
    QString getDateTime();
    ClickableLabel* getUploadLabel();
    ClickableLabel* getPlayLabel();


private:
    Ui::VideoWidget *ui;
    void paintEvent(QPaintEvent *);
    QString dateTime;
    QString thumbnailPath;
    QString fullFilePath;
    MainWindow* myParent;
    ClickableLabel *smallRed;
    ClickableLabel *smallUpload;
    ClickableLabel *smallPlay;


signals:

public slots:


protected:
    //void mousePressEvent ( QMouseEvent * event ) ;

};

#endif // VIDEOWIDGET_H
