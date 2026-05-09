#ifndef FLUXION_CORE_SYSTEMIOS_HPP
#define FLUXION_CORE_SYSTEMIOS_HPP

#include "../System.hpp"
#include "EngineIOS.hpp"

namespace fluxion::core::ios
{
    class System final: public core::System
    {
    public:
        System(int argc, char* argv[]);
        ~System() override = default;

        int run();

        auto getArgumentCount() const { return argumentCount; }
        auto getArguments() const { return arguments; }

        void start();

    private:
        int argumentCount;
        char** arguments;

        std::unique_ptr<Engine> engine;
    };
}

#endif // FLUXION_CORE_SYSTEMIOS_HPP
