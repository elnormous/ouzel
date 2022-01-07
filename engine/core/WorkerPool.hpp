// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_WORKERPOOL_HPP
#define OUZEL_CORE_WORKERPOOL_HPP

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>
#include "../thread/Thread.hpp"
#include "../utils/Log.hpp"

namespace ouzel::core
{
    class WorkerPool final
    {
    public:
        WorkerPool()
        {
            const std::size_t cpuCount = std::thread::hardware_concurrency();
            const std::size_t count = (cpuCount > 1) ? cpuCount - 1 : 1;

            for (unsigned int i = 0; i < count; ++i)
                workers.emplace_back(&WorkerPool::work, this);
        }

        ~WorkerPool()
        {
            running = false;
            taskQueueCondition.notify_all();
        }

        void addTask(std::function<void()> task)
        {
            std::unique_lock lock{taskQueueMutex};
            taskQueue.push(std::move(task));
            lock.unlock();

            taskQueueCondition.notify_one();
        }

    private:
        void work()
        {
            log(Log::Level::info) << "Worker started";

            for (;;)
            {
                std::unique_lock lock{taskQueueMutex};
                taskQueueCondition.wait(lock, [this]() noexcept { return !running || !taskQueue.empty(); });
                if (!running) break;
                const auto task = std::move(taskQueue.front());
                taskQueue.pop();
                lock.unlock();

                task();
            }

            log(Log::Level::info) << "Worker finished";
        }

        std::vector<thread::Thread> workers;
        bool running = true;
        std::queue<std::function<void()>> taskQueue;
        std::mutex taskQueueMutex;
        std::condition_variable taskQueueCondition;
    };
}

#endif // OUZEL_CORE_WORKERPOOL_HPP
