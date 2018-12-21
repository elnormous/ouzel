// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"
#include <string>

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
#  include <unistd.h>
#endif

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#  include <sys/syslog.h>
#endif

#if OUZEL_PLATFORM_WINDOWS
#  include <Windows.h>
#  include <strsafe.h>
#endif

#if OUZEL_PLATFORM_ANDROID
#  include <android/log.h>
#endif

#if OUZEL_PLATFORM_EMSCRIPTEN
#  include <emscripten.h>
#endif

#include "Log.hpp"

namespace ouzel
{
    Log::~Log()
    {
        if (!s.empty())
            logger.log(s, level);
    }

    void Logger::logString(const std::string& str, Log::Level level) const
    {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
        int fd = 0;
        switch (level)
        {
            case Log::Level::ERR:
            case Log::Level::WARN:
                fd = STDERR_FILENO;
                break;
            case Log::Level::INFO:
            case Log::Level::ALL:
                fd = STDOUT_FILENO;
                break;
            default: break;
        }

        std::vector<char> output(str.begin(), str.end());
        output.push_back('\n');

        size_t offset = 0;
        while (offset < output.size())
        {
            ssize_t written = write(fd, output.data() + offset, output.size() - offset);
            if (written == -1)
                return;

            offset += static_cast<size_t>(written);
        }

#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        int priority = 0;
        switch (level)
        {
            case Log::Level::ERR: priority = LOG_ERR; break;
            case Log::Level::WARN: priority = LOG_WARNING; break;
            case Log::Level::INFO: priority = LOG_INFO; break;
            case Log::Level::ALL: priority = LOG_DEBUG; break;
            default: break;
        }
        syslog(priority, "%s", str.c_str());
#elif OUZEL_PLATFORM_WINDOWS
        int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        if (bufferSize == 0)
            return;

        ++bufferSize; // for the newline
        std::vector<WCHAR> buffer(bufferSize);
        if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer.data(), bufferSize) == 0)
            return;

        StringCchCatW(buffer.data(), buffer.size(), L"\n");
        OutputDebugStringW(buffer.data());

#  if DEBUG
        HANDLE handle = 0;
        switch (level)
        {
            case Log::Level::ERR:
            case Log::Level::WARN:
                handle = GetStdHandle(STD_ERROR_HANDLE);
                break;
            case Log::Level::INFO:
            case Log::Level::ALL:
                handle = GetStdHandle(STD_OUTPUT_HANDLE);
                break;
            default: break;
        }

        if (handle)
        {
            DWORD bytesWritten;
            WriteConsoleW(handle, buffer.data(), static_cast<DWORD>(wcslen(buffer.data())), &bytesWritten, nullptr);
        }
#  endif
#elif OUZEL_PLATFORM_ANDROID
        int priority = 0;
        switch (level)
        {
            case Log::Level::ERR: priority = ANDROID_LOG_ERROR; break;
            case Log::Level::WARN: priority = ANDROID_LOG_WARN; break;
            case Log::Level::INFO: priority = ANDROID_LOG_INFO; break;
            case Log::Level::ALL: priority = ANDROID_LOG_DEBUG; break;
            default: break;
        }
        __android_log_print(priority, "Ouzel", "%s", str.c_str());
#elif OUZEL_PLATFORM_EMSCRIPTEN
        int flags = EM_LOG_CONSOLE;
        if (level == Log::Level::ERR) flags |= EM_LOG_ERROR;
        else if (level == Log::Level::WARN) flags |= EM_LOG_WARN;
        emscripten_log(flags, "%s", str.c_str());

#endif
    }

#if OUZEL_MULTITHREADED
    void Logger::logLoop()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            while (running && logQueue.empty()) logCondition.wait(lock);
            if (!running) break;
            std::pair<Log::Level, std::string> str = std::move(logQueue.front());
            logQueue.pop();
            lock.unlock();

            logString(str.second, str.first);
        }
    }
#endif
}
