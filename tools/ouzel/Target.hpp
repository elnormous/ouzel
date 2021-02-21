// Ouzel by Elviss Strazdins

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

        const Platform platform;
        const std::string name;
    };
}

#endif // OUZEL_TARGET_HPP
