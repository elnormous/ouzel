// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <system_error>
#if defined(_WIN32)
#  include <Windows.h>
#else
#  include <pthread.h>
#endif
#include "Utils.hpp"

#if defined(_WIN32)
static constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
#endif

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID && OUZEL_SUPPORTS_NEON_CHECK
    AnrdoidNEONChecker anrdoidNEONChecker;
#endif

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
        if (error != 0)
            throw std::system_error(error, std::system_category(), "Failed to set thread name");
#  elif defined(__linux__) || defined(__ANDROID__)
        int error = pthread_setname_np(pthread_self(), name.c_str());
        if (error != 0)
            throw std::system_error(error, std::system_category(), "Failed to set thread name");
#  endif
#endif
    }
}
