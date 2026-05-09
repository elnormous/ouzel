#ifndef FLUXION_CORE_SYSTEMLINUX_HPP
#define FLUXION_CORE_SYSTEMLINUX_HPP

#include "../System.hpp"
#include "EngineLinux.hpp"

namespace fluxion::core::linux
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

#endif // FLUXION_CORE_SYSTEMLINUX_HPP
