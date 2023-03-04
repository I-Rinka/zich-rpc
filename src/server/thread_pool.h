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

        // Get task
        {
            _idle_threads_num++;
            if (_idle_threads_num == _threads_num)
            {
                statues = ThreadPoolStatues::idle;
            }

            // Wait for tasks

            std::unique_lock<std::mutex> lock(_tasks_queue_mtx);
            _tasks_cv.wait(lock, [this]
                           { return !_tasks_queue.empty() || statues == ThreadPoolStatues::stop; });

            // Stop process
            if (statues == ThreadPoolStatues::stop)
            {
                return;
            }

            // thread is running
            _idle_threads_num--;
            statues = ThreadPoolStatues::running;

            task = _tasks_queue.front();
            _tasks_queue.pop_back();
        }

        try
        {
            task();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Worker thread " << std::this_thread::get_id() << " get error: " << e.what() << std::endl;
        }

        return thread_process();
    }

public:
    std::atomic<ThreadPoolStatues> statues;

    ThreadPool(int thread_num) : statues(ThreadPoolStatues::idle), _threads_num(thread_num), _idle_threads_num(0)
    {
        for (int i = 0; i < _threads_num; i++)
        {
            _thread_pool.push_back(
                std::thread(thread_process));
        }
    };

    ~ThreadPool()
    {
        while (_idle_threads_num != _threads_num && !_tasks_queue.empty())
        {
        }

        statues = ThreadPoolStatues::stop;
        _tasks_cv.notify_all();

        for (auto &th : _thread_pool)
        {
            th.join();
        }
    };

    template <typename F, typename... Args>
    auto ProcessTask(F function, Args... args) -> typename std::future<typename function_traits<F>::return_type>
    {
        
    }
};