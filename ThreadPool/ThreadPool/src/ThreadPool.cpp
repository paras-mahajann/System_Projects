#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t threadCount)
    : stop_(false)
{
    if (threadCount == 0)
    {
        threadCount = 1;
    }

    workers_.reserve(threadCount);

    for (std::size_t i = 0; i < threadCount; ++i)
    {
        workers_.emplace_back(&ThreadPool::workerLoop, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }

    condition_.notify_all();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}



void ThreadPool::workerLoop()
{
    while (true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            condition_.wait(lock, [this]
                {
                    return stop_ || !tasks_.empty();
                });

            if (stop_ && tasks_.empty())
            {
                return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        task();
    }
}