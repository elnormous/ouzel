// Ouzel by Elviss Strazdins

#ifndef OUZEL_THREAD_THREAD_HPP
#define OUZEL_THREAD_THREAD_HPP

#include <system_error>
#include <thread>
#ifdef _WIN32
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <Windows.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#elif defined(__unix__) || defined(__APPLE__)
#  include <pthread.h>
#endif

namespace ouzel::thread
{
    class Thread final
    {
    public:
        Thread() noexcept = default;

        Thread(const Thread&) = delete;
        Thread(Thread&& other) noexcept: t{std::move(other.t)} {}

        Thread(const std::thread&) = delete;
        Thread(std::thread&& other) noexcept: t{std::move(other)} {}

        Thread& operator=(const Thread&) = delete;
        Thread& operator=(Thread&& other) noexcept
        {
            t = std::move(other.t);
            return *this;
        }

        Thread& operator=(const std::thread&) = delete;
        Thread& operator=(std::thread&& other) noexcept
        {
            if (t.joinable()) t.join();
            t = std::move(other);
            return *this;
        }

        template <typename Callable, typename... Args>
        explicit Thread(Callable&& f, Args&&... args):
            t{std::forward<Callable>(f), std::forward<Args>(args)...}
        {
        }

        ~Thread()
        {
            if (t.joinable()) t.join();
        }

        auto isJoinable() const noexcept { return t.joinable(); }
        auto getId() const noexcept { return t.get_id(); }
        auto getNativeHandle() { return t.native_handle(); }

        void join()
        {
            t.join();
        }

        void setPriority(float priority, [[maybe_unused]] bool realtime)
        {
#ifdef _MSC_VER
            static constexpr int priorities[] = {
                THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL,
                THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL
            };

            if (!SetThreadPriority(t.native_handle(), priorities[static_cast<std::size_t>((priority + 1.0F) * 6.0F / 2.0F)]))
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to set thread name"};
#else
            if (priority < 0.0F) priority = 0.0F;

            const auto policy = realtime ? SCHED_RR : SCHED_OTHER;

            const auto minPriority = sched_get_priority_min(policy);
            if (minPriority == -1)
                throw std::system_error{errno, std::system_category(), "Failed to get thread min priority"};
            const auto maxPriority = sched_get_priority_max(policy);
            if (maxPriority == -1)
                throw std::system_error{errno, std::system_category(), "Failed to get thread max priority"};

            sched_param param;
            param.sched_priority = static_cast<int>(priority * static_cast<float>(maxPriority - minPriority)) + minPriority;
            if (const auto error = pthread_setschedparam(t.native_handle(), policy, &param); error != 0)
                throw std::system_error{error, std::system_category(), "Failed to set thread priority"};
#endif
        }

    private:
        std::thread t;
    };
}

#endif // OUZEL_THREAD_THREAD_HPP
