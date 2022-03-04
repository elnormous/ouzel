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
        friend class Promise;
    public:
        void wait()
        {
            std::unique_lock lock{sharedState->mutex};
            sharedState->condition.wait(lock, [this]() noexcept { return sharedState->count == 0; });
        }

    private:
        struct State final
        {
        public:
            State(std::size_t c): count{c}
            {
            }

            std::size_t count = 0;
            std::mutex mutex;
            std::condition_variable condition;
        };

        Future(std::shared_ptr<State> state) noexcept:
            sharedState{state}
        {
        }

        std::shared_ptr<State> sharedState;
    };

    class Promise final
    {
    public:
        Promise(const TaskGroup& taskGroup) noexcept:
            sharedState{std::make_shared<Future::State>(taskGroup.getTaskCount())}
        {
        }

        Future getFuture() const
        {
            return Future{sharedState};
        }

        void decrement()
        {
            std::unique_lock lock{sharedState->mutex};

            if (sharedState->count == 0 || --sharedState->count == 0)
            {
                lock.unlock();
                sharedState->condition.notify_all();
            }
        }

    private:
        std::shared_ptr<Future::State> sharedState;
    };

    class WorkerPool final
    {
    public:
        WorkerPool()
        {
            const std::size_t cpuCount = std::thread::hardware_concurrency();
            const std::size_t count = (cpuCount > 1) ? cpuCount - 1 : 1;

            for (std::size_t i = 0; i < count; ++i)
                workers.emplace_back(&WorkerPool::work, this);
        }

        ~WorkerPool()
        {
            running = false;
            taskQueueCondition.notify_all();
        }

        Future run(TaskGroup&& taskGroup)
        {
            Promise promise{taskGroup};
            Future future = promise.getFuture();

            std::unique_lock lock{taskQueueMutex};

            while (!taskGroup.taskQueue.empty())
            {
                taskQueue.push(std::pair(std::move(promise),
                                         std::move(taskGroup.taskQueue.front())));
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

                task.first.decrement();
            }

            log(Log::Level::info) << "Worker finished";
        }

        std::vector<thread::Thread> workers;
        bool running = true;
        std::queue<std::pair<Promise, std::function<void()>>> taskQueue;
        std::mutex taskQueueMutex;
        std::condition_variable taskQueueCondition;
    };
}

#endif // OUZEL_CORE_WORKERPOOL_HPP
