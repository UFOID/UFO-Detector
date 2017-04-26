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
    m_capacity = capacity;
}

VideoBuffer::~VideoBuffer()
{

}

BufferedVideoFrame* VideoBuffer::waitNextFrame() {
    if (!m_buffer.isEmpty()) {
        return m_buffer.takeFirst();
    }
    return NULL;
}

int VideoBuffer::count() {
    return 0;
}

int VideoBuffer::capacity() {
    return m_capacity;
}

bool VideoBuffer::pushFrame(BufferedVideoFrame *frame) {
    m_buffer.append(frame);
    return true;
}

void VideoBuffer::stopWait() {
}


