// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_SYSTEMLINUX_HPP
#define OUZEL_CORE_SYSTEMLINUX_HPP

#include "../System.hpp"
#include "EngineLinux.hpp"

namespace ouzel::core::linux
{
    class System final: public core::System
    {
    public:
        System(int argc, char* argv[]);
        ~System() override = default;

        void run();

    private:
        Engine engine;
    };
}

#endif // OUZEL_CORE_SYSTEMLINUX_HPP
