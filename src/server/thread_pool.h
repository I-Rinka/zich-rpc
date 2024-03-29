#include <future>
#include <functional>
#include <thread>
#include <list>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <iostream>
#include "../util/function_traits.h"

#ifndef __ZICHRPC_THREAD_POOL__
#define __ZICHRPC_THREAD_POOL__

enum class ThreadPoolStatues
{
    idle,
    stop,
    running,
};

class ThreadPool
{
private:
    int _threads_num;

    std::list<std::function<void()>> _tasks_queue;
    std::list<std::thread> _thread_pool;
    std::mutex _tasks_queue_mtx;
    std::condition_variable _tasks_cv;

    std::atomic<int> _idle_threads_num;

    void thread_process()
    {
        std::function<void()> task;

        _idle_threads_num++;
        if (_idle_threads_num == _threads_num)
        {
            // It should be an atomatic operation. When status is stop, the value should not be set
            // Only set when it is running
            auto expected = ThreadPoolStatues::running;
            statues.compare_exchange_strong(expected, ThreadPoolStatues::idle);
        }

        // Wait for tasks
        {
            std::unique_lock<std::mutex> lock(_tasks_queue_mtx);

            // Get task
            _tasks_cv.wait(lock, [this]
                           { return !_tasks_queue.empty() || statues == ThreadPoolStatues::stop; });

            // Stop thread pool
            if (statues == ThreadPoolStatues::stop)
            {
                return;
            }

            // If threadpool set status to stop here but one thread just pass the judgement
            // let it go but make sure it never set status to other status other than stop.

            // thread is running
            _idle_threads_num--;
            auto expected = ThreadPoolStatues::idle;
            statues.compare_exchange_strong(expected, ThreadPoolStatues::running);

            task = _tasks_queue.front();
            _tasks_queue.pop_front();
        }

        // No try catch is needed. The only issue is future itself is violated. Which means
        // a task is executed twice
        task();

        return thread_process();
    }

public:
    std::atomic<ThreadPoolStatues> statues;

    ThreadPool(int thread_num) : statues(ThreadPoolStatues::idle), _threads_num(thread_num), _idle_threads_num(0)
    {
        for (int i = 0; i < _threads_num; i++)
        {
            _thread_pool.push_back(
                std::thread([this]
                            { thread_process(); }));
        }
    };

    ~ThreadPool()
    {
        while (_idle_threads_num != _threads_num && !_tasks_queue.empty()) // All threads are idle and no more tasks
        {
        }

        statues = ThreadPoolStatues::stop;
        _tasks_cv.notify_all();

        for (auto &th : _thread_pool)
        {
            th.join();
        }
    };

    int GetWorkingThreadsNum() const noexcept
    {
        return _threads_num - _idle_threads_num;
    }

    template <typename F, typename... Args>
    auto AddTask(F &&function, Args &&...args) -> typename std::future<typename function_traits<F>::return_type>
    {
        using return_type = typename function_traits<F>::return_type;

        // Task is not assignable, so make it on the stack
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(function), std::forward<Args>(args)...));

        {
            std::lock_guard<std::mutex> lock(_tasks_queue_mtx);
            _tasks_queue.emplace_back([=]
                                      { (*task)(); });
        }

        _tasks_cv.notify_one();

        return task->get_future();
    }

    template <typename F, typename... Args>
    auto ProcessTask(F function, Args &&...args) -> typename function_traits<F>::return_type
    {
        auto future = AddTask(function, std::forward<Args>(args)...);
        return future.get();
    }
};
#endif