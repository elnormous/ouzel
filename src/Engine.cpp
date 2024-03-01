#include "Engine.hpp"

namespace ouzel
{
	void Engine::run()
	{
		const auto threadCount = std::thread::hardware_concurrency();

		for (size_t i = 0; i < threadCount; ++i)
			workerThreads.emplace_back(&Engine::worker, this);

		while (running)
		{
			
		}
	}

	void Engine::schedule(std::function<void()> task)
	{
		std::unique_lock lock{taskQueueMutex};
		taskQueue.push(std::move(task));
		lock.unlock();
		taskQueueCondition.notify_one();
	}

	void Engine::worker()
	{
		for (;;)
		{
			std::unique_lock lock{taskQueueMutex};
			taskQueueCondition.wait(lock, [this]{ return !taskQueue.empty() || !running; });

			if (!running) return;

			const auto task = std::move(taskQueue.front());
			taskQueue.pop();
			lock.unlock();

			task();
		}
	}
}