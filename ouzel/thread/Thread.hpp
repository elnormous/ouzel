// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include <memory>
#include <string>
#if defined(_WIN32)
#include <Windows.h>
#define ThreadLocal __declspec(thread)
#else
#include <pthread.h>
#define ThreadLocal __thread
#if defined(__APPLE__)
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif // #if defined(__APPLE__)
#endif

namespace ouzel
{
    inline uint32_t getCPUCount()
    {
#if defined(_WIN32)
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
#else
#if defined(__APPLE__)
        int mib[2];
        mib[0] = CTL_HW;
#ifdef HW_AVAILCPU
        mib[1] = HW_AVAILCPU;
#else
        mib[1] = HW_NCPU;
#endif
        int count;
        size_t size = sizeof(count);
        sysctl(mib, 2, &count, &size, NULL, 0);
        return (count > 0) ? static_cast<uint32_t>(count) : 0;
#elif defined(__linux__) || defined(__ANDROID__)
        int count = sysconf(_SC_NPROCESSORS_ONLN);
        return (count > 0) ? static_cast<uint32_t>(count) : 0;
#else
        return 1;
#endif
#endif
    }

    class Thread final
    {
    public:
        class ID
        {
            friend Thread;
        public:
            bool operator==(const ID& other)
            {
#if defined(_WIN32)
                return threadId == other.threadId;
#else
                return pthread_equal(thread, other.thread) != 0;
#endif
            }

            bool operator!=(const ID& other)
            {
#if defined(_WIN32)
                return threadId != other.threadId;
#else
                return pthread_equal(thread, other.thread) == 0;
#endif
            }

        protected:
#if defined(_WIN32)
            ID(DWORD id):
                threadId(id)
#else
            ID(pthread_t t):
                thread(t)
#endif
            {
            }
        private:
#if defined(_WIN32)
            DWORD threadId = 0;
#else
            pthread_t thread = 0;
#endif
        };

        Thread() {}
        explicit Thread(const std::function<void()>& function, const std::string& name = "");

        ~Thread();

        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;

        Thread(Thread&& other);
        Thread& operator=(Thread&& other);

        bool run();
        bool join();

        int32_t getPriority() const
        {
#if defined(_WIN32)
            return GetThreadPriority(handle);
#else
            int policy;
            sched_param param;
            if (pthread_getschedparam(thread, &policy, &param) == 0) return param.sched_priority;
            return 0;
#endif
        }

        bool setPriority(int32_t priority)
        {
#if defined(_WIN32)
            return SetThreadPriority(handle, priority) != 0;
#else
            sched_param param;
            param.sched_priority = priority;
            return pthread_setschedparam(thread, SCHED_RR, &param) == 0;
#endif
        }

        inline bool isJoinable() const
        {
#if defined(_WIN32)
            return handle != nullptr;
#else
            return thread != 0;
#endif
        }

        ID getId() const
        {
#if defined(_WIN32)
            return threadId;
#else
            return thread;
#endif
        }

        static ID getCurrentThreadId()
        {
#if defined(_WIN32)
            return GetCurrentThreadId();
#else
            return pthread_self();
#endif
        }

        static void setCurrentThreadName(const std::string& name);

        struct State
        {
            std::function<void()> function;
            std::string name;
        };

    protected:
        std::unique_ptr<State> state;

#if defined(_WIN32)
        HANDLE handle = nullptr;
        DWORD threadId = 0;
#else
        pthread_t thread = 0;
#endif
    };
}
