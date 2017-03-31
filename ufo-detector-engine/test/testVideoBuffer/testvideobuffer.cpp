
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
        frame->m_skippedFrames = (i + 1);

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
        QVERIFY(frame->m_skippedFrames == (TEST_VIDEO_BUFFER_CAPACITY - i + 1));
        QVERIFY(m_videoBuffer->m_freeSlots->available() == (TEST_VIDEO_BUFFER_CAPACITY - i + 1));
        QVERIFY(m_videoBuffer->m_reservedSlots->available() == i - 1);
    }
}

QTEST_APPLESS_MAIN(TestVideoBuffer)

#include "testvideobuffer.moc"
