// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_WORKERPOOL_HPP
#define OUZEL_CORE_WORKERPOOL_HPP

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include "../thread/Thread.hpp"
#include "../utils/Log.hpp"

namespace ouzel::core
{
    class TaskGroup final
    {
        friend class WorkerPool;
    public:
        TaskGroup() = default;

        void add(std::function<void()> task)
        {
            taskQueue.push(std::move(task));
        }

        std::size_t getTaskCount() const noexcept { return taskQueue.size(); }

    private:
        std::queue<std::function<void()>> taskQueue;
    };

    class Future final
    {
        friend class WorkerPool;
    public:
        Future(const TaskGroup& taskGroup) noexcept:
            data{std::make_shared<Data>(taskGroup.getTaskCount())}
        {
        }

        void wait()
        {
            data->wait();
        }

    private:
        bool finishTask()
        {
            return data->finishTask();
        }

        class Data final
        {
        public:
            Data(std::size_t count): taskCount{count}
            {
            }

            void wait()
            {
                std::unique_lock lock{taskMutex};
                taskCondition.wait(lock, [this]() noexcept { return taskCount == 0; });
            }

            bool finishTask()
            {
                std::unique_lock lock{taskMutex};

                if (--taskCount == 0)
                {
                    lock.unlock();
                    taskCondition.notify_all();
                    return true;
                }

                return false;
            }

        private:
            std::size_t taskCount = 0;
            std::mutex taskMutex;
            std::condition_variable taskCondition;
        };

        std::shared_ptr<Data> data;
    };

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

        Future run(TaskGroup&& taskGroup)
        {
            Future future{taskGroup};

            std::unique_lock lock{taskQueueMutex};

            taskGroupQueue.push(std::pair(future, std::move(taskGroup.taskQueue)));

            lock.unlock();

            taskQueueCondition.notify_all();

            return future;
        }

    private:
        void work()
        {
            log(Log::Level::info) << "Worker started";

            for (;;)
            {
                std::unique_lock lock{taskQueueMutex};
                taskQueueCondition.wait(lock, [this]() noexcept { return !running || !taskGroupQueue.empty(); });
                if (!running) break;
                auto& taskGroup = taskGroupQueue.front();
                const auto task = std::move(taskGroup.second.front());
                taskGroup.second.pop();
                lock.unlock();

                task();

                if (taskGroup.first.finishTask())
                {
                    lock.lock();
                    taskGroupQueue.pop();
                }
            }

            log(Log::Level::info) << "Worker finished";
        }

        std::vector<thread::Thread> workers;
        bool running = true;
        std::queue<std::pair<Future, std::queue<std::function<void()>>>> taskGroupQueue;
        std::mutex taskQueueMutex;
        std::condition_variable taskQueueCondition;
    };
}

#endif // OUZEL_CORE_WORKERPOOL_HPP
