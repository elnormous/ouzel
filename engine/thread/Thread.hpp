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
#include "../utils/Bit.hpp"

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

        void setPriority(float priority, bool realtime)
        {
#ifdef _MSC_VER
            static_cast<void>(realtime);
            static constexpr int priorities[] = {
                THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL,
                THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL
            };

            if (!SetThreadPriority(t.native_handle(), priorities[static_cast<std::size_t>((priority + 1.0F) * 6.0F / 2.0F)]))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to set thread name");
#else
            if (priority < 0.0F) priority = 0.0F;

            const auto policy = realtime ? SCHED_RR : SCHED_OTHER;

            const auto minPriority = sched_get_priority_min(policy);
            if (minPriority == -1)
                throw std::system_error(errno, std::system_category(), "Failed to get thread min priority");
            const auto maxPriority = sched_get_priority_max(policy);
            if (maxPriority == -1)
                throw std::system_error(errno, std::system_category(), "Failed to get thread max priority");

            sched_param param;
            param.sched_priority = static_cast<int>(priority * static_cast<float>(maxPriority - minPriority)) + minPriority;
            if (const auto error = pthread_setschedparam(t.native_handle(), policy, &param); error != 0)
                throw std::system_error(error, std::system_category(), "Failed to set thread priority");
#endif
        }

    private:
        std::thread t;
    };

    inline void setCurrentThreadName(const std::string& name)
    {
#ifdef _MSC_VER
        constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
#  pragma pack(push,8)
        typedef struct tagTHREADNAME_INFO
        {
            DWORD dwType; // Must be 0x1000.
            LPCSTR szName; // Pointer to name (in user addr space).
            DWORD dwThreadID; // Thread ID (-1=caller thread).
            DWORD dwFlags; // Reserved for future use, must be zero.
        } THREADNAME_INFO;
#  pragma pack(pop)

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name.c_str();
        info.dwThreadID = static_cast<DWORD>(-1);
        info.dwFlags = 0;

        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0,
                           sizeof(info) / sizeof(ULONG_PTR),
                           bitCast<ULONG_PTR*>(&info));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
#else
#  ifdef __APPLE__
        const auto error = pthread_setname_np(name.c_str());
        if (error != 0)
            throw std::system_error(error, std::system_category(), "Failed to set thread name");
#  elif defined(__linux__)
        const auto error = pthread_setname_np(pthread_self(), name.c_str());
        if (error != 0)
            throw std::system_error(error, std::system_category(), "Failed to set thread name");
#  endif
#endif
    }
}

#endif // OUZEL_THREAD_THREAD_HPP
