// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_HPP
#define OUZEL_PLATFORM_HPP

#include <stdexcept>
#include <string>

namespace ouzel
{
    enum class Platform
    {
        windows,
        macOs,
        linux,
        iOs,
        tvOs,
        android,
        emscripten
    };

    inline std::string toString(Platform platform)
    {
        switch (platform)
        {
            case Platform::windows: return "Windows";
            case Platform::macOs: return "macOS";
            case Platform::linux: return "Linux";
            case Platform::iOs: return "iOS";
            case Platform::tvOs: return "tvOS";
            case Platform::android: return "Android";
            case Platform::emscripten: return "Emscripten";
        }

        throw std::runtime_error{"Unknown platform"};
    }

    inline Platform stringToPlatform(const std::string& s)
    {
        if (s == "windows")
            return Platform::windows;
        else if (s == "macos")
            return Platform::macOs;
        else if (s == "linux")
            return Platform::linux;
        else if (s == "ios")
            return Platform::iOs;
        else if (s == "tvos")
            return Platform::tvOs;
        else if (s == "android")
            return Platform::android;
        else if (s == "emscripten")
            return Platform::emscripten;
        else
            throw std::runtime_error{"Invalid platform"};
    }
}

#endif // OUZEL_PLATFORM_HPP
