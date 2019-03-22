// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <system_error>
#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  include <Windows.h>
#  undef WIN32_LEAN_AND_MEAN
#  undef NOMINMAX
#else
#  include <pthread.h>
#endif
#include "Utils.hpp"

#if defined(_WIN32)
static constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
#  pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#  pragma pack(pop)
#endif

namespace ouzel
{
    std::mt19937 randomEngine(std::random_device{}());

    void setCurrentThreadName(const std::string& name)
    {
#if defined(_MSC_VER)
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name.c_str();
        info.dwThreadID = static_cast<DWORD>(-1);
        info.dwFlags = 0;

        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
#else
#  ifdef __APPLE__
        int error = pthread_setname_np(name.c_str());
#  elif defined(__linux__)
        int error = pthread_setname_np(pthread_self(), name.c_str());
#  endif
        if (error != 0)
            throw std::system_error(error, std::system_category(), "Failed to set thread name");
#endif
    }

    void setThreadPriority(std::thread& t, float priority, bool realtime)
    {
#if defined(_MSC_VER)
        (void)realtime;
        int priorities[] = {
            THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL,
            THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL
        };

        if (!SetThreadPriority(t.native_handle(), priorities[static_cast<size_t>((priority + 1.0F) * 6.0F / 2.0F)]))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to set thread name");
#else
        if (priority < 0.0F) priority = 0.0F;

        int policy = realtime ? SCHED_RR : SCHED_OTHER;

        int minPriority = sched_get_priority_min(policy);
        if (minPriority == -1)
            throw std::system_error(errno, std::system_category(), "Failed to get thread min priority");
        int maxPriority = sched_get_priority_max(policy);
        if (maxPriority == -1)
            throw std::system_error(errno, std::system_category(), "Failed to get thread max priority");

        sched_param param;
        param.sched_priority = static_cast<int>(priority * (maxPriority - minPriority)) + minPriority;
        int error = pthread_setschedparam(t.native_handle(), policy, &param);
        if (error != 0)
            throw std::system_error(error, std::system_category(), "Failed to set thread priority");

#endif
    }
}
