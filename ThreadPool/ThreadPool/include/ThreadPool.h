#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <type_traits>
#include <utility>
#include <memory>
#include <stdexcept>


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
    ThreadPool& operator=(ThreadPool&&) =  delete;

    //submit the task
    template<typename F,typename... Args>
    auto submit(F&& task,Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
    {

        using ReturnType = std::invoke_result_t<F, Args...>;
        auto packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(
            [
                task = std::forward<F>(task),
                ...args = std::forward<Args>(args)
            ]() mutable {
                return std::invoke(std::move(task),std::move(args)...);
            }
        );

        std::future<ReturnType> future = packagedTask->get_future();

        {   
            std::lock_guard<std::mutex>lock(queue_mutex_);          
            if (stop_)
            {
                throw std::runtime_error("ThreadPool has been stopped");
            }

            tasks_.emplace(
                [packagedTask]() {
                    (*packagedTask)();
                }
            );
        }

        condition_.notify_one();
        return future;
    }

    [[nodiscard]]
    std::size_t workerCount() const noexcept;

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    mutable std::mutex queue_mutex_;
    std::condition_variable condition_;

    bool stop_ = false;
};