#include <mutex>
#include <string>
#include "../include/ConcurrentQueue.h"

using namespace std;

std::string ConcurrentQueue::pop() {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
        cond_.wait(mlock);
    }
    auto item = queue_.front();
    queue_.pop();
    return item;
}

void ConcurrentQueue::pop(std::string &item) {
    std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        item = queue_.front();
        queue_.pop();
}

void ConcurrentQueue::push(const std::string &item) {
    std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
        mlock.unlock();
        cond_.notify_one();
}

