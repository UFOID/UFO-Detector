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
#include <QString>
#include <QtTest>


#define TEST_VIDEO_BUFFER_CAPACITY 20

class TestVideoBuffer : public QObject
{
    Q_OBJECT

public:
    TestVideoBuffer();

    VideoBuffer* m_videoBuffer;

private Q_SLOTS:
    void capacity();
    void getAndSetNoBlock();
};

TestVideoBuffer::TestVideoBuffer() {
    m_videoBuffer = new VideoBuffer(TEST_VIDEO_BUFFER_CAPACITY);
    QVERIFY(NULL != m_videoBuffer);
}

void TestVideoBuffer::capacity() {
    QVERIFY(TEST_VIDEO_BUFFER_CAPACITY == m_videoBuffer->m_capacity);
    QVERIFY(TEST_VIDEO_BUFFER_CAPACITY == m_videoBuffer->capacity());
}

void TestVideoBuffer::getAndSetNoBlock() {
    BufferedVideoFrame* frame = NULL;

    QVERIFY(false == m_videoBuffer->pushFrame(NULL));

    for (int i = 0; i < TEST_VIDEO_BUFFER_CAPACITY; i++) {
        frame = new BufferedVideoFrame;
        frame->m_frame = NULL;
        frame->m_duplicateCount = (i + 1);

        QVERIFY(m_videoBuffer->count() == i);
        QVERIFY(m_videoBuffer->m_buffer.count() == m_videoBuffer->count());
        QVERIFY(m_videoBuffer->m_freeSlots->available() == (TEST_VIDEO_BUFFER_CAPACITY - i));
        QVERIFY(m_videoBuffer->m_reservedSlots->available() == i);

        QVERIFY(m_videoBuffer->pushFrame(frame));

        //QVERIFY(m_videoBuffer->m_buffer.at(i) == frame);
        QVERIFY(m_videoBuffer->count() == (i + 1));
        QVERIFY(m_videoBuffer->m_buffer.count() == m_videoBuffer->count());
        QVERIFY(m_videoBuffer->m_freeSlots->available() == (TEST_VIDEO_BUFFER_CAPACITY - i - 1));
        QVERIFY(m_videoBuffer->m_reservedSlots->available() == (i + 1));
    }

    for (int i = TEST_VIDEO_BUFFER_CAPACITY; i > 0; i--) {
        QVERIFY(m_videoBuffer->count() == i);
        QVERIFY(m_videoBuffer->m_buffer.count() == m_videoBuffer->count());
        QVERIFY(m_videoBuffer->m_freeSlots->available() == (TEST_VIDEO_BUFFER_CAPACITY - i));
        QVERIFY(m_videoBuffer->m_reservedSlots->available() == i);

        frame = m_videoBuffer->waitNextFrame();

        QVERIFY(NULL != frame);
        //QVERIFY(m_videoBuffer->m_buffer.at(i) == frame);
        QVERIFY(m_videoBuffer->count() == (i - 1));
        QVERIFY(m_videoBuffer->m_buffer.count() == m_videoBuffer->count());
        QVERIFY(frame->m_frame == NULL);
        QVERIFY(frame->m_duplicateCount == (TEST_VIDEO_BUFFER_CAPACITY - i + 1));
        QVERIFY(m_videoBuffer->m_freeSlots->available() == (TEST_VIDEO_BUFFER_CAPACITY - i + 1));
        QVERIFY(m_videoBuffer->m_reservedSlots->available() == i - 1);
    }
}

QTEST_APPLESS_MAIN(TestVideoBuffer)

#include "testvideobuffer.moc"
