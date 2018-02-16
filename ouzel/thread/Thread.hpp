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
        class ID
        {
            friend Thread;
        public:
            bool operator==(const ID& other)
            {
#if defined(_MSC_VER)
                return threadId == other.threadId;
#else
                return pthread_equal(thread, other.thread) != 0;
#endif
            }

            bool operator!=(const ID& other)
            {
#if defined(_MSC_VER)
                return threadId != other.threadId;
#else
                return pthread_equal(thread, other.thread) == 0;
#endif
            }

        protected:
#if defined(_MSC_VER)
            ID(DWORD id):
                threadId(id)
#else
            ID(pthread_t t):
                thread(t)
#endif
            {
            }
        private:
#if defined(_MSC_VER)
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

        inline bool isJoinable() const
        {
#if defined(_MSC_VER)
            return handle != nullptr;
#else
            return thread != 0;
#endif
        }

        ID getId() const
        {
#if defined(_MSC_VER)
            return ID(threadId);
#else
            return ID(thread);
#endif
        }

        static ID getCurrentThreadId()
        {
#if defined(_MSC_VER)
            return ID(GetCurrentThreadId());
#else
            return ID(pthread_self());
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
        HANDLE handle = nullptr;
        DWORD threadId = 0;
#else
        pthread_t thread = 0;
#endif
    };
}
