#ifndef FLUXION_TARGET_HPP
#define FLUXION_TARGET_HPP

#include <string>
#include "Platform.hpp"

namespace fluxion
{
    class Target final
    {
    public:
        Target(Platform initPlatform,
               const std::string& initName):
            platform{initPlatform}, name{initName} {}

        const Platform platform;
        const std::string name;
    };
}

#endif // FLUXION_TARGET_HPP
