// Ouzel by Elviss Strazdins

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif

#ifdef __ANDROID__
#  include <android/log.h>
#elif TARGET_OS_IOS || TARGET_OS_TV
#  include <sys/syslog.h>
#elif TARGET_OS_MAC || defined(__linux__)
#  include <unistd.h>
#elif defined(_WIN32)
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
#  include <strsafe.h>
#elif defined(__EMSCRIPTEN__)
#  include <emscripten.h>
#endif

#include "Log.hpp"

namespace ouzel
{
#ifdef __ANDROID__
    constexpr int getPriority(Log::Level level) noexcept
    {
        switch (level)
        {
            case Log::Level::error: return ANDROID_LOG_ERROR;
            case Log::Level::warning: return ANDROID_LOG_WARN;
            case Log::Level::info: return ANDROID_LOG_INFO;
            case Log::Level::all: return ANDROID_LOG_DEBUG;
            default: return ANDROID_LOG_DEFAULT;
        }
    }
#elif TARGET_OS_IOS || TARGET_OS_TV
    constexpr int getPriority(Log::Level level) noexcept
    {
        switch (level)
        {
            case Log::Level::error: return LOG_ERR;
            case Log::Level::warning: return LOG_WARNING;
            case Log::Level::info: return LOG_INFO;
            case Log::Level::all: return LOG_DEBUG;
            default: return 0;
        }
    }
#elif TARGET_OS_MAC || defined(__linux__)
    constexpr int getFd(Log::Level level) noexcept
    {
        switch (level)
        {
            case Log::Level::error:
            case Log::Level::warning:
                return STDERR_FILENO;
            case Log::Level::info:
            case Log::Level::all:
                return STDOUT_FILENO;
            default: return STDOUT_FILENO;
        }
    }
#elif defined(__EMSCRIPTEN__)
    constexpr int getFlags(Log::Level level) noexcept
    {
        switch (level)
        {
            case Log::Level::error: return EM_LOG_CONSOLE | EM_LOG_ERROR;
            case Log::Level::warning: return EM_LOG_CONSOLE | EM_LOG_WARN;
            case Log::Level::info:
            case Log::Level::all:
                return EM_LOG_CONSOLE
            default: return EM_LOG_CONSOLE
        }
    }
#elif defined(_WIN32) && DEBUG
    HANDLE getHandle(Log::Level level) noexcept
    {
        switch (level)
        {
            case Log::Level::error:
            case Log::Level::warning:
                return GetStdHandle(STD_ERROR_HANDLE);
            case Log::Level::info:
            case Log::Level::all:
                return GetStdHandle(STD_OUTPUT_HANDLE);
            default: return INVALID_HANDLE_VALUE;
        }
    }
#endif

    void Logger::logString(const std::string& str, Log::Level level)
    {
#ifdef __ANDROID__
        __android_log_print(getPriority(level), "Ouzel", "%s", str.c_str());
#elif TARGET_OS_IOS || TARGET_OS_TV
        syslog(getPriority(level), "%s", str.c_str());
#elif TARGET_OS_MAC || defined(__linux__)
        const int fd = getFd(level);
        std::vector<char> output(str.begin(), str.end());
        output.push_back('\n');

        std::size_t offset = 0;
        while (offset < output.size())
        {
            auto written = write(fd, output.data() + offset, output.size() - offset);
            while (written == -1 && errno == EINTR)
                written = write(fd, output.data() + offset, output.size() - offset);

            if (written == -1)
                return;

            offset += static_cast<std::size_t>(written);
        }
#elif defined(__EMSCRIPTEN__)
        emscripten_log(getFlags(level), "%s", str.c_str());
#elif defined(_WIN32)
        const auto bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        if (bufferSize == 0)
            return;

        auto buffer = std::make_unique<WCHAR[]>(bufferSize + 1); // +1 for the newline
        if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer.get(), bufferSize) == 0)
            return;

        if (FAILED(StringCchCatW(buffer.get(), static_cast<size_t>(bufferSize + 1), L"\n")))
            return;

        OutputDebugStringW(buffer.get());
#  if DEBUG
        const HANDLE handle = getHandle(level);
        if (handle == INVALID_HANDLE_VALUE)
            return;

        WriteConsoleW(handle, buffer.get(), static_cast<DWORD>(wcslen(buffer.get())), nullptr, nullptr);
#  endif
#endif
    }
}
