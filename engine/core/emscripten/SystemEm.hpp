#ifndef FLUXION_CORE_SYSTEMEM_HPP
#define FLUXION_CORE_SYSTEMEM_HPP

#include "../System.hpp"
#include "EngineEm.hpp"

namespace fluxion::core::emscripten
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

#endif // FLUXION_CORE_SYSTEMEM_HPP
