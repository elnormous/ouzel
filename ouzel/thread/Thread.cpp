// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Thread.hpp"

#if defined(_WIN32)
static const DWORD MS_VC_EXCEPTION = 0x406D1388;
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

#if defined(_WIN32)
static DWORD WINAPI threadFunction(LPVOID parameter)
#else
static void* threadFunction(void* parameter)
#endif
{
    ouzel::Thread::State* state = static_cast<ouzel::Thread::State*>(parameter);

    if (!state->name.empty()) ouzel::Thread::setCurrentThreadName(state->name);

    state->function();

#if defined(_WIN32)
    return 0;
#else
    return NULL;
#endif
}

namespace ouzel
{
    Thread::Thread(const std::function<void()>& function, const std::string& name):
        state(new State())
    {
        state->function = function;
        state->name = name;

#if defined(_WIN32)
        handle = CreateThread(nullptr, 0, threadFunction, state.get(), 0, &threadId);
        if (handle == nullptr) return;
#else
        if (pthread_create(&thread, NULL, threadFunction, state.get()) != 0) return;
#endif
    }

    Thread::~Thread()
    {
#if defined(_WIN32)
        if (handle)
        {
            WaitForSingleObject(handle, INFINITE);
            CloseHandle(handle);
        }
#else
        if (thread) pthread_join(thread, nullptr);
#endif
    }

    Thread::Thread(Thread&& other)
    {
#if defined(_WIN32)
        handle = other.handle;
        threadId = other.threadId;
        other.handle = nullptr;
        other.threadId = 0;
#else
        thread = other.thread;
        other.thread = 0;
#endif
        state = std::move(other.state);
    }

    Thread& Thread::operator=(Thread&& other)
    {
        if (&other != this)
        {
#if defined(_WIN32)
            if (handle)
            {
                WaitForSingleObject(handle, INFINITE);
                CloseHandle(handle);
            }
            handle = other.handle;
            threadId = other.threadId;
            other.handle = nullptr;
            other.threadId = 0;
#else
            if (thread) pthread_join(thread, nullptr);
            thread = other.thread;
            other.thread = 0;
#endif
            state = std::move(other.state);
        }

        return *this;
    }

    bool Thread::join()
    {
#if defined(_WIN32)
        return handle ? (WaitForSingleObject(handle, INFINITE) != WAIT_FAILED) : false;
#else
        return thread ? (pthread_join(thread, nullptr) == 0) : false;
#endif
    }

    bool Thread::setCurrentThreadName(const std::string& name)
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
        return true;
#else
#ifdef __APPLE__
        return pthread_setname_np(name.c_str()) == 0;
#elif defined(__linux__) || defined(__ANDROID__)
        return pthread_setname_np(pthread_self(), name.c_str()) == 0;
#else
        return true;
#endif
#endif
    }
}
