#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <utility>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include<functional>


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

    template<typename F,typename... Args>
    void submit(F&& task,Args&&... args){
        {
            std::lock_guard<std::mutex>lock(queue_mutex_);
            auto wrapper =
                [
                    task = std::forward<F>(task),
                    ... args = std::forward<Args>(args)
                ]() mutable
                {
                    std::invoke(task,args...);
                };

            tasks_.emplace(std::move(wrapper));
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