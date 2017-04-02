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
#include <QThread>
#include <QtTest>
#include <QTest>


#define TEST_VIDEO_BUFFER_CAPACITY 20

/**
 * @brief Frame reader class for helping VideoBuffer unit test
 */
class FrameReader : public QObject {
    Q_OBJECT
public:
    FrameReader(VideoBuffer* buffer);
private:
    VideoBuffer* m_buffer;
    BufferedVideoFrame* m_readFrame;
public slots:
    void readNextFrame();
signals:
    void frameRead(BufferedVideoFrame* frame);
};

FrameReader::FrameReader(VideoBuffer* buffer) {
    m_buffer = buffer;
}

void FrameReader::readNextFrame() {
    m_readFrame = m_buffer->waitNextFrame();
    emit frameRead(m_readFrame);
}

/**
 * @brief Frame bufferer class for helping VideoBuffer unit test
 */
class FrameWriter : public QObject {
    Q_OBJECT
public:
    FrameWriter(VideoBuffer* buffer);
private:
    VideoBuffer* m_buffer;
public slots:
    void bufferNextFrame(BufferedVideoFrame* frame);
signals:
    void frameBuffered(bool success);
};

FrameWriter::FrameWriter(VideoBuffer* buffer) {
    m_buffer = buffer;
}

void FrameWriter::bufferNextFrame(BufferedVideoFrame* frame) {
    bool ok = m_buffer->pushFrame(frame);
    emit frameBuffered(ok);
}


/**
 * @brief VideoBuffer unit test class
 */
class TestVideoBuffer : public QObject
{
    Q_OBJECT

public:
    TestVideoBuffer();

public slots:
    void onFrameRead(BufferedVideoFrame* frame);  ///< frame read by FrameReader helper class
    void onFrameBuffered(bool success); ///< frame buffered by FrameWriter helper class

signals:
    void readFrame();   ///< allow frame reading for FrameReader helper class
    void bufferFrame(BufferedVideoFrame* frame); ///< give frame to FrameWriter helper class

private Q_SLOTS:
    void init();    // fixture
    void cleanup(); // fixture cleanup

    void capacity();
    void getAndSetNoBlock();
    void waitNextFrame_emptyBuffer();
    void pushFrame_fullBuffer();

private:
    VideoBuffer* m_videoBuffer;
    BufferedVideoFrame* m_lastFrame;
    int m_frameCounter;
    bool m_bufferEventSuccessful;
};

TestVideoBuffer::TestVideoBuffer() {
    m_videoBuffer = NULL;
    m_lastFrame = NULL;
    m_frameCounter = 0;
}

void TestVideoBuffer::init() {
    m_videoBuffer = new VideoBuffer(TEST_VIDEO_BUFFER_CAPACITY);
    QVERIFY(NULL != m_videoBuffer);
    m_frameCounter = 0;
}

void TestVideoBuffer::cleanup() {
    if (m_videoBuffer) {
        m_videoBuffer->stopWait();
        m_videoBuffer->deleteLater();
    }
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

void TestVideoBuffer::onFrameRead(BufferedVideoFrame *frame) {
    m_lastFrame = frame;
    m_frameCounter++;
}

void TestVideoBuffer::waitNextFrame_emptyBuffer() {
    FrameReader* frameReader = new FrameReader(m_videoBuffer);
    QThread frameReaderThread;

    QVERIFY(0 == m_frameCounter);

    connect(this, SIGNAL(readFrame()), frameReader, SLOT(readNextFrame()));
    connect(frameReader, SIGNAL(frameRead(BufferedVideoFrame*)), this, SLOT(onFrameRead(BufferedVideoFrame*)));
    frameReader->moveToThread(&frameReaderThread);
    frameReaderThread.start();

    QVERIFY(0 == m_frameCounter);
    QVERIFY(m_videoBuffer->m_freeSlots->available() == TEST_VIDEO_BUFFER_CAPACITY);
    QVERIFY(m_videoBuffer->m_reservedSlots->available() == 0);

    // waitNextFrame() should block when the buffer is empty
    emit readFrame();
    QTest::qWait(200);
    QVERIFY(0 == m_frameCounter);

    m_videoBuffer->stopWait();
    QTest::qWait(200);  // increase if stopWait() doesn't return in time
    QVERIFY(1 == m_frameCounter);
    QVERIFY(NULL == m_lastFrame);
    QVERIFY(m_videoBuffer->m_freeSlots->available() == TEST_VIDEO_BUFFER_CAPACITY);
    QVERIFY(m_videoBuffer->m_reservedSlots->available() == 0);

    frameReaderThread.exit();
    frameReaderThread.wait();
}

void TestVideoBuffer::onFrameBuffered(bool success) {
    m_bufferEventSuccessful = success;
    m_frameCounter++;
}

void TestVideoBuffer::pushFrame_fullBuffer() {
    BufferedVideoFrame* frame = NULL;
    FrameWriter* frameWriter = new FrameWriter(m_videoBuffer);
    QThread frameWriterThread;

    connect(this, SIGNAL(bufferFrame(BufferedVideoFrame*)),
            frameWriter, SLOT(bufferNextFrame(BufferedVideoFrame*)));
    connect(frameWriter, SIGNAL(frameBuffered(bool)), this, SLOT(onFrameBuffered(bool)));

    for (int i = 0; i < TEST_VIDEO_BUFFER_CAPACITY; i++) {
        frame = new BufferedVideoFrame;
        frame->m_frame = NULL;
        frame->m_duplicateCount = 0;
        QVERIFY(m_videoBuffer->count() == i);
        QVERIFY(i == m_frameCounter);
        frameWriter->bufferNextFrame(frame);
        QVERIFY(m_videoBuffer->count() == (i + 1));
        QVERIFY((i + 1) == m_frameCounter);
    }

    frameWriter->moveToThread(&frameWriterThread);
    frameWriterThread.start();

    m_bufferEventSuccessful = true;
    QVERIFY(TEST_VIDEO_BUFFER_CAPACITY == m_frameCounter);

    frame = new BufferedVideoFrame;
    frame->m_frame = NULL;
    frame->m_duplicateCount = 0;
    // pushFrame() should block when buffer is full
    emit bufferFrame(frame);
    QTest::qWait(200);
    QVERIFY(TEST_VIDEO_BUFFER_CAPACITY == m_frameCounter);
    QVERIFY(m_bufferEventSuccessful);

    m_videoBuffer->stopWait();
    QTest::qWait(200);  // increase if stopWait() doesn't return in time
    QVERIFY((TEST_VIDEO_BUFFER_CAPACITY + 1) == m_frameCounter);
    QVERIFY(!m_bufferEventSuccessful);
    QVERIFY(m_videoBuffer->m_freeSlots->available() == 0);
    QVERIFY(m_videoBuffer->m_reservedSlots->available() == TEST_VIDEO_BUFFER_CAPACITY);

    frameWriterThread.exit();
    frameWriterThread.wait();
}

QTEST_MAIN(TestVideoBuffer)

#include "testvideobuffer.moc"
