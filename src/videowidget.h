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

/**
 * @brief video widget to be used as list item
 */
class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = 0, QString filepath = "", QString theDateTime = "", QString videoLength="");
    ~VideoWidget();
    ClickableLabel* deleteButton();
    QString videoFileName();
    QString thumbnailFileName();
    QString dateTime();
    ClickableLabel* uploadButton();
    ClickableLabel* playButton();

private:
    Ui::VideoWidget *ui;
    void paintEvent(QPaintEvent *);
    QString m_dateTime;
    QString m_thumbnailFileName;
    QString m_videoFileName;
    MainWindow* myParent;
    ClickableLabel *m_deleteButton;
    ClickableLabel *m_uploadButton;
    ClickableLabel *m_playButton;


signals:

public slots:

protected:
};

#endif // VIDEOWIDGET_H
