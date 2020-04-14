// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_TARGET_HPP
#define OUZEL_TARGET_HPP

#include <string>
#include "Platform.hpp"

namespace ouzel
{
    class Target final
    {
    public:
        Target(Platform initPlatform,
               const std::string& initName):
            platform{initPlatform}, name(initName) {}

        Platform getPlatform() const noexcept { return platform; }
        const std::string& getName() const noexcept { return name; }

    private:
        Platform platform;
        std::string name;
    };
}

#endif // OUZEL_TARGET_HPP
