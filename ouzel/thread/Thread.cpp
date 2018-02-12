// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Thread.hpp"

#if defined(_MSC_VER)
static DWORD WINAPI threadFunction(LPVOID parameter)
#else
static void* threadFunction(void* parameter)
#endif
{
    ouzel::Thread::Parameters* parameters = static_cast<ouzel::Thread::Parameters*>(parameter);

#ifdef __APPLE__
    if (!parameters->name.empty()) pthread_setname_np(parameters->name.c_str());
#endif

    parameters->function();

#if defined(_MSC_VER)
    return 0;
#else
    return NULL;
#endif
}

namespace ouzel
{
    Thread::Thread(const std::function<void()>& function, const std::string& name)
    {
        parameters.function = function;
        parameters.name = name;

#if defined(_MSC_VER)
        DWORD threadId;
        handle = CreateThread(nullptr, 0, threadFunction, &parameters, 0, &threadId);
        if (thread->handle == nullptr) return;

        if (!name.empty())
        {
            THREADNAME_INFO info;
            info.dwType = 0x1000;
            info.szName = name.c_str();
            info.dwThreadID = threadId;
            info.dwFlags = 0;

            __try
            {
                RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
        }
#else
        if (pthread_create(&thread, NULL, threadFunction, &parameters) != 0) return;

#ifndef __APPLE__
        if (!name.empty()) pthread_setname_np(thread->thread, name.c_str());
#endif
#endif
    }

    Thread::Thread(Thread&& other)
    {
#if defined(_MSC_VER)
        handle = other.handle;
        other.handle = nullptr;
#else
        thread = other.thread;
        other.thread = 0;
#endif
    }

    Thread& Thread::operator=(Thread&& other)
    {
#if defined(_MSC_VER)
        if (handle)
        {
            WaitForSingleObject(handle, INFINITE);
            CloseHandle(handle);
        }
        handle = other.handle;
        other.handle = nullptr;
#else
        if (thread) pthread_join(thread, nullptr);
        thread = other.thread;
        other.thread = 0;
#endif

        return *this;
    }

    Thread::~Thread()
    {
#if defined(_MSC_VER)
        if (handle)
        {
            WaitForSingleObject(handle, INFINITE);
            CloseHandle(handle);
        }
#else
        if (thread) pthread_join(thread, nullptr);
#endif
    }

    bool Thread::join()
    {
#if defined(_MSC_VER)
        return handle ? (WaitForSingleObject(handle, INFINITE) != WAIT_FAILED) : false;
#else
        return thread ? (pthread_join(thread, nullptr) == 0) : false;
#endif
    }
}
