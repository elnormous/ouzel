// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include <memory>
#include <string>
#if defined(_MSC_VER)
#include <Windows.h>
#else
#include <pthread.h>
#endif

namespace ouzel
{
    class Thread
    {
    public:
        Thread() {}
        explicit Thread(const std::function<void()>& function, const std::string& name = "");

        ~Thread();

        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;

        Thread(Thread&& other);
        Thread& operator=(Thread&& other);

        bool run();
        bool join();

        inline uint64_t getId() const
        {
#if defined(_MSC_VER)
            return static_cast<uint64_t>(threadId);
#else
            return reinterpret_cast<uint64_t>(thread);
#endif
        }

        inline bool isJoinable() const
        {
#if defined(_MSC_VER)
            return handle != nullptr;
#else
            return thread != 0;
#endif
        }

        static uint64_t getCurrentThreadId()
        {
#if defined(_MSC_VER)
            return static_cast<uint64_t>(GetCurrentThreadId());
#else
            return reinterpret_cast<uint64_t>(pthread_self());
#endif
        }

        static bool setCurrentThreadName(const std::string& name);

        struct State
        {
            std::function<void()> function;
            std::string name;
        };

    protected:
        std::unique_ptr<State> state;

#if defined(_MSC_VER)
        DWORD threadId = 0;
        HANDLE handle = nullptr;
#else
        pthread_t thread = 0;
#endif
    };
}
