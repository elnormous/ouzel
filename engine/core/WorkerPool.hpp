// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_WORKERPOOL_HPP
#define OUZEL_CORE_WORKERPOOL_HPP

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
            const auto count = std::thread::hardware_concurrency();

            for (unsigned int i = 0; i < count; ++i)
                workers.emplace_back(&WorkerPool::work, this);
        }

    private:
        void work()
        {
            log(Log::Level::info) << "Worker started";
        }

        std::vector<thread::Thread> workers;
    };
}

#endif // OUZEL_CORE_WORKERPOOL_HPP
