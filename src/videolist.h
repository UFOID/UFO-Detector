#ifndef VIDEOLIST_H
#define VIDEOLIST_H

#include <QListWidget>
#include <QMouseEvent>
#include <QDebug>

/**
 * @brief The VideoList class
 *
 * Default behaviour for 2nd mouse button click on item is to change item
 * selection on the list. This class prevents that.
 */
class VideoList : public QListWidget
{
    Q_OBJECT
public:
    VideoList(QWidget* parent = 0);

    void mousePressEvent(QMouseEvent* event);
};

#endif // VIDEOLIST_H
