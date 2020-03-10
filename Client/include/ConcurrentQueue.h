#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>


class ConcurrentQueue {
public:

    std::string pop();

    void pop(std::string& item);

    void push(const std::string& item);

    ConcurrentQueue() = default;

    ConcurrentQueue(const ConcurrentQueue&) = default; // disable copying

    ConcurrentQueue& operator=(const ConcurrentQueue&) = default; // disable assignment

private:
    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;

};

#endif