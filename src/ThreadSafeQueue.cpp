#include <mutex>
#include <deque>
#include <condition_variable>
#include "TimedMat.h"

template<typename T>
class ThreadSafeQueue {
    std::mutex mutex;
    std::condition_variable cond_var;
    std::deque<T> queue;
    size_t max_size;

public:
    ThreadSafeQueue(size_t maxSize) : max_size(maxSize) {}

    void push(T &&item) {
        {
            std::lock_guard lock(mutex);
            if (queue.size() >= max_size) {
                // todo this should not be done in ThreadSafeQueue
                TimedMat temp = queue.front();
                temp.frame.release();
                queue.pop_front();
            }
            queue.push_back(item);
        }

        cond_var.notify_one();
    }

//    T& front() {
//        std::unique_lock lock(mutex);
//        cond_var.wait(lock, [&]{ return !queue.empty(); });
//        return queue.front();
//    }
//
//    void pop() {
//        std::lock_guard lock(mutex);
//        queue.pop();
//    }

    T frontAndPop() {
        std::unique_lock lock(mutex);
        cond_var.wait(lock, [&]{ return !queue.empty(); });
        T item = queue.front();
        queue.pop_front();
        return item;
    }

    void clear() {
        std::lock_guard lock(mutex);
        queue.clear();
    }

    size_t size() {
        std::lock_guard lock(mutex);
        return queue.size();
    }

    void setMaxSize(size_t maxSize) {
        max_size = maxSize;
    }
};