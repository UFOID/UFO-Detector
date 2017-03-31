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

#include "videobuffer.h"

VideoBuffer::VideoBuffer(int capacity, QObject *parent) : QObject(parent) {
    m_buffer.clear();
    m_capacity = capacity;
    m_freeSlots = new QSemaphore(m_capacity);
    m_reservedSlots = new QSemaphore(0);
    m_waitingEnabled = true;
    m_waitTimeoutMs = 100;
}

BufferedVideoFrame* VideoBuffer::waitNextFrame() {
    BufferedVideoFrame* frame = NULL;
    bool slotAcquired = false;
    while (m_waitingEnabled && !slotAcquired) {
        slotAcquired = m_reservedSlots->tryAcquire(1, m_waitTimeoutMs);
    }
    if (!m_buffer.isEmpty()) {
        frame = m_buffer.dequeue();
    }
    m_freeSlots->release(1);
    return frame;
}

bool VideoBuffer::pushFrame(BufferedVideoFrame* frame) {
    bool ok = false;
    bool slotAcquired = false;
    if (!frame) {
        return false;
    }
    while (m_waitingEnabled && !slotAcquired) {
        slotAcquired = m_freeSlots->tryAcquire(1, m_waitTimeoutMs);
    }
    if (m_buffer.count() < m_capacity) {
        m_buffer.enqueue(frame);
        ok = true;
    } else {
        ok = false;
    }
    m_reservedSlots->release(1);
    return ok;
}

int VideoBuffer::capacity() {
    return m_capacity;
}

int VideoBuffer::count() {
    return m_buffer.size();
}

void VideoBuffer::stopWait() {
    m_waitingEnabled = false;
}
