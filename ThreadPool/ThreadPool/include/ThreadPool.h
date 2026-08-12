#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <utility>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    explicit ThreadPool(
        std::size_t threadCount = std::thread::hardware_concurrency());

    ~ThreadPool();

    // Non-copyable
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Movable
    ThreadPool(ThreadPool&&) noexcept;
    ThreadPool& operator=(ThreadPool&&) noexcept;

    template <typename F>
    void submit(F&& task){
        {
            std::lock_guard<std::mutex>lock(queue_mutex_);
            tasks_.emplace(std::forward<F>(task));
        }

        condition_.notify_one();

    }

    [[nodiscard]]
    std::size_t workerCount() const noexcept;

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    mutable std::mutex queue_mutex_;
    std::condition_variable condition_;

    bool stop_;
};