// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_SYSTEMTVOS_HPP
#define OUZEL_CORE_SYSTEMTVOS_HPP

#include "../System.hpp"

namespace ouzel::core::tvos
{
    class System final: public core::System
    {
    public:
        System(int argc, char* argv[]);
        ~System() override = default;
    };
}

#endif // OUZEL_CORE_SYSTEMTVOS_HPP
