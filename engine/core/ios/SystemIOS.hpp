// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_SYSTEMIOS_HPP
#define OUZEL_CORE_SYSTEMIOS_HPP

#include "../System.hpp"

namespace ouzel::core::ios
{
    class System final: public core::System
    {
    public:
        System(int argc, char* argv[]);
        ~System() override = default;
    };
}

#endif // OUZEL_CORE_SYSTEMIOS_HPP
