// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_UTILS_LOG_HPP
#define OUZEL_UTILS_LOG_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include "math/Matrix4.hpp"
#include "math/Quaternion.hpp"
#include "math/Size2.hpp"
#include "math/Size3.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"

namespace ouzel
{
    class Logger;

    class Log final
    {
    public:
        enum class Level
        {
            OFF,
            ERR,
            WARN,
            INFO,
            ALL
        };

        explicit Log(const Logger& initLogger, Level initLevel = Level::INFO):
            logger(initLogger), level(initLevel)
        {
        }

        Log(const Log& other):
            logger(other.logger)
        {
            level = other.level;
            s = other.s;
        }

        Log(Log&& other):
            logger(other.logger)
        {
            level = other.level;
            other.level = Level::INFO;
            s = std::move(other.s);
        }

        Log& operator=(const Log& other)
        {
            level = other.level;
            s = other.s;

            return *this;
        }

        Log& operator=(Log&& other)
        {
            if (&other != this)
            {
                level = other.level;
                other.level = Level::INFO;
                s = std::move(other.s);
            }

            return *this;
        }

        ~Log();

        template<typename T> Log& operator<<(T val)
        {
            s += std::to_string(val);
            return *this;
        }

        Log& operator<<(const std::string& val)
        {
            s += val;
            return *this;
        }

        Log& operator<<(const char* val)
        {
            s += val;

            return *this;
        }

        Log& operator<<(char* val)
        {
            s += val;
            return *this;
        }

        Log& operator<<(const std::vector<std::string>& val)
        {
            bool first = true;

            for (const std::string& str : val)
            {
                if (!first) s += ", ";
                first = false;
                s += str;
            }

            return *this;
        }

        Log& operator<<(const Matrix4& val)
        {
            s += std::to_string(val.m[0]) + "," + std::to_string(val.m[1]) + "," + std::to_string(val.m[2]) + "," + std::to_string(val.m[3]) + "\n" +
                std::to_string(val.m[4]) + "," + std::to_string(val.m[5]) + "," + std::to_string(val.m[6]) + "," + std::to_string(val.m[7]) + "\n" +
                std::to_string(val.m[8]) + "," + std::to_string(val.m[9]) + "," + std::to_string(val.m[10]) + "," + std::to_string(val.m[11]) + "\n" +
                std::to_string(val.m[12]) + "," + std::to_string(val.m[13]) + "," + std::to_string(val.m[14]) + "," + std::to_string(val.m[15]);

            return *this;
        }

        Log& operator<<(const Quaternion& val)
        {
            s += std::to_string(val.x) + "," + std::to_string(val.y) + "," +
                std::to_string(val.z) + "," + std::to_string(val.w);

            return *this;
        }

        Log& operator<<(const Size2& val)
        {
            s += std::to_string(val.width) + "," + std::to_string(val.height);
            return *this;
        }

        Log& operator<<(const Size3& val)
        {
            s += std::to_string(val.width) + "," + std::to_string(val.height) + "," +
                std::to_string(val.depth);

            return *this;
        }

        Log& operator<<(const Vector2& val)
        {
            s += std::to_string(val.x) + "," + std::to_string(val.y);

            return *this;
        }

        Log& operator<<(const Vector3& val)
        {
            s += std::to_string(val.x) + "," + std::to_string(val.y) + "," +
                std::to_string(val.z);

            return *this;
        }

        Log& operator<<(const Vector4& val)
        {
            s += std::to_string(val.x) + "," + std::to_string(val.y) + "," +
                std::to_string(val.z) + "," + std::to_string(val.w);

            return *this;
        }

    private:
        const Logger& logger;
        Level level = Level::INFO;
        std::string s;
    };

    class Logger final
    {
    public:
        explicit Logger(Log::Level initThreshold = Log::Level::ALL):
            threshold(initThreshold)
        {
#if OUZEL_MULTITHREADED
            logThread = std::thread(&Logger::logLoop, this);
#endif
        }

        ~Logger()
        {
#if OUZEL_MULTITHREADED
            std::unique_lock<std::mutex> lock(queueMutex);
            running = false;
            lock.unlock();
            logCondition.notify_all();
#endif
        }

        Log log(Log::Level level = Log::Level::INFO) const
        {
            return Log(*this, level);
        }

        void log(const std::string& str, Log::Level level = Log::Level::INFO) const
        {
#if OUZEL_MULTITHREADED
            std::unique_lock<std::mutex> lock(queueMutex);
            logQueue.push(std::make_pair(level, str));
            lock.unlock();
            logCondition.notify_all();
#else
            logString(str, level);
#endif
        }

    private:
        void logString(const std::string& str, Log::Level level = Log::Level::INFO) const;

#ifdef DEBUG
        std::atomic<Log::Level> threshold{Log::Level::ALL};
#else
        std::atomic<Log::Level> threshold{Log::Level::INFO};
#endif

#if OUZEL_MULTITHREADED
        void logLoop();

        mutable std::condition_variable logCondition;
        mutable std::mutex queueMutex;
        mutable std::queue<std::pair<Log::Level, std::string>> logQueue;
        std::thread logThread;
        bool running = true;
#endif
    };
}

#endif // OUZEL_UTILS_LOG_HPP
