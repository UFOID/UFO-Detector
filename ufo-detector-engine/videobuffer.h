/*
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

#ifndef VIDEOBUFFER_H
#define VIDEOBUFFER_H

#include <QObject>
#include <QQueue>
#include <QSemaphore>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

struct BufferedVideoFrame {
    cv::Mat* m_frame;       ///< pointer to video frame
    int m_skippedFrames;    ///< how many frames were skipped before this frame
};

/**
 * @brief Video buffer class with thread-safe access
 */
class VideoBuffer : public QObject
{
    Q_OBJECT
public:
    explicit VideoBuffer(int capacity, QObject *parent = 0);

    /**
     * @brief Wait next frame and pop it from the buffer. Will block in case of empty buffer.
     * Call to stopWait() method will stop the waiting. In that case, this method
     * will return NULL.
     * @return pointer to BufferedVideoFrame
     */
    BufferedVideoFrame* waitNextFrame();

    /**
     * @brief Push frame to the tail of buffer. Will block in case of full buffer.
     * Call to stopWait() method will stop the blocking. In that case, this method
     * will return false.
     * @param frame pointer to frame
     * @return true if succeeded, false if not (was interrupted by stopWait())
     */
    bool pushFrame(BufferedVideoFrame* frame);

    /**
     * @brief Capacity of buffer.
     * @return
     */
    int capacity();

    /**
     * @brief The current number of frames in the buffer.
     * @return
     */
    int count();

    /**
     * @brief Stop all waiting in case of empty or full buffer
     * This method will stop execution of waitNextFrame() if it's currently blocked,
     * waiting new video frames. Also the execution of pushFrame() is stopped if
     * it's currently blocked, waiting free space in the buffer.
     */
    void stopWait();

#ifndef _UNIT_TEST_
private:
#endif
    QQueue<BufferedVideoFrame*> m_buffer;
    int m_capacity;
    QSemaphore* m_freeSlots;
    QSemaphore* m_reservedSlots;
    bool m_waitingEnabled;  ///< blocking enabled on full/empty buffer
    int m_waitTimeoutMs;    ///< interval at which waiting enabled flag is checked

signals:

public slots:
};

#endif // VIDEOBUFFER_H
