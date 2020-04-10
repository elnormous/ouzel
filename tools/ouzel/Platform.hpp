// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_PLATFORM_HPP
#define OUZEL_PLATFORM_HPP

#include <stdexcept>
#include <string>

namespace ouzel
{
    enum class Platform
    {
        Windows,
        MacOs,
        Linux,
        Ios,
        Tvos,
        Android,
        Emscripten
    };

    inline std::string toString(Platform platform)
    {
        switch (platform)
        {
            case Platform::Windows: return "Windows";
            case Platform::MacOs: return "macOS";
            case Platform::Linux: return "Linux";
            case Platform::Ios: return "iOS";
            case Platform::Tvos: return "tvOS";
            case Platform::Android: return "Android";
            case Platform::Emscripten: return "Emscriptem";
        }

        throw std::runtime_error("Unknown platform");
    }
}

#endif // OUZEL_PLATFORM_HPP
