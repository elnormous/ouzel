#ifndef FLUXION_CORE_SYSTEMTVOS_HPP
#define FLUXION_CORE_SYSTEMTVOS_HPP

#include "../System.hpp"
#include "EngineTVOS.hpp"

namespace fluxion::core::tvos
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

#endif // FLUXION_CORE_SYSTEMTVOS_HPP
