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
            sharedState{std::make_shared<State>(taskGroup.getTaskCount())}
        {
        }

        void wait()
        {
            sharedState->wait();
        }

    private:
        void finishTask()
        {
            sharedState->finishTask();
        }

        class State final
        {
        public:
            State(std::size_t count): taskCount{count}
            {
            }

            void wait()
            {
                std::unique_lock lock{taskMutex};
                taskCondition.wait(lock, [this]() noexcept { return taskCount == 0; });
            }

            void finishTask()
            {
                std::unique_lock lock{taskMutex};

                if (--taskCount == 0)
                {
                    lock.unlock();
                    taskCondition.notify_all();
                }
            }

        private:
            std::size_t taskCount = 0;
            std::mutex taskMutex;
            std::condition_variable taskCondition;
        };

        std::shared_ptr<State> sharedState;
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

            while (!taskGroup.taskQueue.empty())
            {
                taskQueue.push(std::pair(future, std::move(taskGroup.taskQueue.front())));
                taskGroup.taskQueue.pop();
            }

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
                taskQueueCondition.wait(lock, [this]() noexcept { return !running || !taskQueue.empty(); });
                if (!running) break;
                auto task = std::move(taskQueue.front());
                taskQueue.pop();
                lock.unlock();

                task.second();

                task.first.finishTask();
            }

            log(Log::Level::info) << "Worker finished";
        }

        std::vector<thread::Thread> workers;
        bool running = true;
        std::queue<std::pair<Future, std::function<void()>>> taskQueue;
        std::mutex taskQueueMutex;
        std::condition_variable taskQueueCondition;
    };
}

#endif // OUZEL_CORE_WORKERPOOL_HPP
