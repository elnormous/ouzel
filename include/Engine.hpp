#ifndef OUZEL_ENGINE_HPP
#define OUZEL_ENGINE_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <queue>
#include <vector>

namespace ouzel
{
	class Engine
	{
	public:
		void run();

		void schedule(std::function<void()> task);

	private:
		void worker();

		std::vector<std::jthread> workerThreads;

		std::queue<std::function<void()>> taskQueue;
		std::mutex taskQueueMutex;
		std::condition_variable taskQueueCondition;

		std::atomic_bool running = true;
	};
}

#endif