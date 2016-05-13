#include "videolist.h"

VideoList::VideoList(QWidget *parent) :
    QListWidget(parent)
{

}

void VideoList::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        // don't let right button click through to VideoList item
        event->accept();
    } else {
        QListWidget::mousePressEvent(event);
    }
}
