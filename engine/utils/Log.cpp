// Ouzel by Elviss Strazdins

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if defined(__ANDROID__)
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
    Logger logger;

    void Logger::logString(const std::string& str, Log::Level level)
    {
#if defined(__ANDROID__)
        int priority = 0;
        switch (level)
        {
            case Log::Level::error: priority = ANDROID_LOG_ERROR; break;
            case Log::Level::warning: priority = ANDROID_LOG_WARN; break;
            case Log::Level::info: priority = ANDROID_LOG_INFO; break;
            case Log::Level::all: priority = ANDROID_LOG_DEBUG; break;
            default: return;
        }
        __android_log_print(priority, "Ouzel", "%s", str.c_str());
#elif TARGET_OS_IOS || TARGET_OS_TV
        int priority = 0;
        switch (level)
        {
            case Log::Level::error: priority = LOG_ERR; break;
            case Log::Level::warning: priority = LOG_WARNING; break;
            case Log::Level::info: priority = LOG_INFO; break;
            case Log::Level::all: priority = LOG_DEBUG; break;
            default: return;
        }
        syslog(priority, "%s", str.c_str());
#elif TARGET_OS_MAC || defined(__linux__)
        int fd = 0;
        switch (level)
        {
            case Log::Level::error:
            case Log::Level::warning:
                fd = STDERR_FILENO;
                break;
            case Log::Level::info:
            case Log::Level::all:
                fd = STDOUT_FILENO;
                break;
            default: return;
        }

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
        HANDLE handle = INVALID_HANDLE_VALUE;
        switch (level)
        {
            case Log::Level::error:
            case Log::Level::warning:
                handle = GetStdHandle(STD_ERROR_HANDLE);
                break;
            case Log::Level::info:
            case Log::Level::all:
                handle = GetStdHandle(STD_OUTPUT_HANDLE);
                break;
            default: return;
        }

        if (handle == INVALID_HANDLE_VALUE)
            return;

        WriteConsoleW(handle, buffer.get(), static_cast<DWORD>(wcslen(buffer.get())), nullptr, nullptr);
#  endif
#elif defined(__EMSCRIPTEN__)
        int flags = EM_LOG_CONSOLE;
        switch (level)
        {
            case Log::Level::error:
                flags |= EM_LOG_ERROR;
                break;
            case Log::Level::warning:
                flags |= EM_LOG_WARN;
                break;
            case Log::Level::info:
            case Log::Level::all:
                break;
            default: return;
        }
        emscripten_log(flags, "%s", str.c_str());
#endif
    }
}
